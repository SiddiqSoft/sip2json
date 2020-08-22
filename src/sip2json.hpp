/*
	A SIP Parser for Modern C++
	Version 1.0.0
	https://github.com/siddiqsoftware/sip2json/

	BSD 3-Clause License

	Copyright (c) 2003-2020, Abdelkareem Siddiq
	All rights reserved.

	Redistribution and use in source and binary forms, with or without
	modification, are permitted provided that the following conditions are met:

	1. Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.

	2. Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.

	3. Neither the name of the copyright holder nor the names of its
	contributors may be used to endorse or promote products derived from
	this software without specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
	AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
	IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
	DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
	FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
	DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
	SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
	CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
	OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <algorithm>
#include <string>
#include <regex>
#include <memory>
#include <iterator>
#include <chrono>
#include <random>
#include <sstream>
#include <functional>
#include <optional>

#define FMT_HEADER_ONLY 1
#include "fmt/chrono.h"
#include "nlohmann/json.hpp"

#include "sip2json_exception.hpp"
#include "sip2json_response_codes.hpp"
#include "sip2json_utils.hpp"
#include "sipmessage.hpp"


namespace siddiqsoftware
{
#pragma region SIP match patterns
#pragma endregion

	/// @brief SIP message encoder and decoder utility class
	class sip2json
	{
	public:
#pragma region Parsing helpers
	private:
		/// @brief Parse the start line
		/// @param sipm Destination sipmessage
		/// @param bufferStart Start of the stream.
		/// @param bufferEnd End of the stream
		/// @return true/false depending on the state of the decode of start line.
		static bool
		parseStartLine(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			using namespace std;

			match_results<string::iterator> matchStartLine;

			auto found = regex_search(bufferStart, bufferEnd, matchStartLine, SIP_PATTERN_STARTLINE);
			if (found && (matchStartLine.size() >= 3))
			{
				// The regex is very precise and there is no chance we will end up here
				// with an ill-formed (or unsupported) start-line.
				if (SIPVER_20.compare(matchStartLine[3]) == 0)
				{
					sipm["s"s] = {{"type"s, SIPMessageType::request},
								  {"method"s, matchStartLine[1]},
								  {"uri"s, matchStartLine[2]},
								  {"version"s, matchStartLine[3]}};
				}
				else if (SIPVER_20.compare(matchStartLine[1]) == 0)
				{
					sipm["s"s] = {{"type"s, SIPMessageType::response},
								  {"reason"s, matchStartLine[3]},
								  {"status"s, stoi(matchStartLine[2].str())},
								  {"version"s, matchStartLine[1]}};
				}

				// Offset the start to the point after the start-line. Make sure to skip over any prefix!
				// We may have junk or left-over crud at the start (especially if we're using text files)
				bufferStart += (matchStartLine.length() + matchStartLine.prefix().length() + ELEM_NEWLINE.size());
			}
			else
			{
				sip2json_throw<invalid_startline_error>("{} - SIP Startline not found.", __func__);
			}

			return found;
		}


		/// @brief Store the value in the header section. Performs from basic transforms/detection of bool, integer
		/// @param sipm The target sipmessage object
		/// @param key The key
		/// @param value The value
		/// @return Returns true if the store was successful.
		static bool storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value)
		{
			if (key.find(HF_VIA) == 0)
			{
				// Via is an array
				sipm["h"][HF_VIA].push_back(value);
			}
			else if (_stricmp(key.c_str(), "uthorization") == 0)
			{
				// Some encoders send "uthorization" instead of "Authorization"
				sipm["h"][HF_AUTHORIZATION] = value;
			}
			else if (_stricmp(key.c_str(), HF_CONTENT_TYPE.c_str()) == 0)
			{
				// Some encoders send Content-type instead of the standard Content-Type; here we need to normalize it.
				sipm["h"][HF_CONTENT_TYPE] = value;
			}
			else if (key.find(HF_CONTENT_LENGTH) == 0)
			{
				sipm["h"][key] = std::stoi(value);
			}
			else if (key.find(HF_EXPIRES) == 0)
			{
				sipm["h"][key] = std::stoi(value);
			}
			// This helper causes issues when the payload may contain "true" or "false" as a string value not intended as boolean
			// This approach allows the client the ultimate authority for decoding the data.
			//else if (value.find("true") == 0)
			//{
			//	sipm["h"][key] = true;
			//}
			//else if (value.find("false") == 0)
			//{
			//	sipm["h"][key] = false;
			//}
			else if (value.empty())
			{
				sipm["h"][key] = "";
			}
			else
			{
				sipm["h"][key] = value;
			}

			return true;
		}


		/// @brief Decode headers within the stream
		/// @param sipm Destination sipmessage
		/// @param bufferStart Start of the buffer. Just past the end of the start line section (tip of the header section).
		/// @param bufferEnd End of the stream
		/// @return true/false depending on the state of the decode of headers.
		static bool
		parseHeaders(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			bool done  = false;
			bool found = false;

			// WARNING
			// The bufferStart must point to the start of the first sequence (excluding the CRLF) after the startline is processed!
			// Scan for the location of the header section end within the frame.
			// If we don't have one, then we should bail out.
			// Note that for response messages, it is likely that the bufferEnd will also be the headerEnd (no content).
			auto headerEnd =
					std::search(bufferStart, bufferEnd, ELEM_HEADERSECTIONDELIMITER.begin(), ELEM_HEADERSECTIONDELIMITER.end());

			// Assert header end delimiter must exist!
			sip2json_throw_if<incomplete_buffer_for_header_error>(size_t(bufferEnd - headerEnd) <
																		  ELEM_HEADERSECTIONDELIMITER.size(),
																  "{}:Cannot find header section delimiter.",
																  __func__);

			while (!done)
			{
				// Scan for the first `:`
				auto hsep = std::search(bufferStart, headerEnd, ELEM_SEPERATOR.begin(), ELEM_SEPERATOR.end());
				if (hsep != headerEnd)
				{
					// Found the separator element.
					// Key is from bufferStart until the separator
					if (std::string key(bufferStart, hsep); !key.empty())
					{
						std::string value {};
						auto		hval = hsep; // Store the location of the value part of the header element.

						// Next, let's look for the end of element
						bufferStart = hsep += ELEM_SEPERATOR.size();

						// Skip over the leading "space" if found.
						if (*bufferStart == ' ') bufferStart = ++hsep;

					label_recummulate_to_unfold_buffer:
						auto hend = search(hsep, headerEnd, ELEM_NEWLINE.begin(), ELEM_NEWLINE.end());
						if (hend != headerEnd)
						{
							// We found the `\r\n`;
							// Next, check if this is a folded element
							if ((headerEnd != (hend + ELEM_NEWLINE.size() + 1)) &&
								((*(hend + ELEM_NEWLINE.size() + 1) == ' ') ||
								 ((*hend + ELEM_NEWLINE.size() + 1) == '\t'))) // peek ahead to see if we have.. folded indicator
							{
								// Yes, we have a folded item.
								// build up the value..
								value.append(hsep, hend);
								// Advance to past the fold portion
								hsep = hend + ELEM_NEWLINE.size() + 1;
								// Go back to find the next section..
								goto label_recummulate_to_unfold_buffer;
							}
							else
							{
								value.append(hsep, hend);
								found		= storeHeaderValue(sipm, key, value);
								bufferStart = hend += ELEM_NEWLINE.size();
							}
						}
						else
						{
							// reached the end; We're done
							value.append(hsep, hend);
							found		= storeHeaderValue(sipm, key, value);
							bufferStart = headerEnd + ELEM_HEADERSECTIONDELIMITER.size();
							done		= true;
						}
					}
					else
					{
						// Key is empty; we're done.
						done = true;
					}
				}
				else
				{
					// End of buffer or Could not find separator; we're done.
					done = true;
				}
			}

			return found;
		}


		/// @brief Decode a SDP message blocks
		/// @param sipm Destination sipmessage
		/// @param bufferStart Start of the buffer. Just past the end of the header section (tip of the content section).
		/// @param bufferEnd End of the content area (not the end of the stream)
		/// @return true/false depending on the state of the decode of SDP blocks.
		static bool
		parseBodySDP(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			using namespace std;

			match_results<string::iterator> matcher;
			bool							found = false;

			// NOTE: bufferStart points to the location past the very first v=0 as this is the signal of the start
			// of the body. Therefore, we start with blockIndex = 0 and then increment everytime we encounter next v=0
			int32_t blockIndex = -1;

			while ((bufferStart < bufferEnd) && regex_search(bufferStart, bufferEnd, matcher, SIP_PATTERN_BODY))
			{
				if (matcher.size() == 3)
				{
					auto key   = matcher[1].str();
					auto value = matcher[2].str();

					found = true;
					if (key == "v"s)
					{
						// First element; increment blockIndex.
						// Add the next element to a new SDP object.
						blockIndex++; // the first match will increment this to "0"
						//nlohmann::json::json_pointer pkey(fmt::format("/b/sdp/{}/{}", blockIndex, key));
						//sipm[pkey]						   = 0;
						sipm["b"s]["sdp"s][blockIndex][key] = 0;
					}
					else if (key == "a"s)
					{
						// attribute lines: https://en.wikipedia.org/wiki/Session_Description_Protocol#Attributes
						//sipm[pkey].push_back(matcher[2].str());
						match_results<string::iterator> alineMatcher;
						if (regex_search(value.begin(), value.end(), alineMatcher, SIP_PATTERN_BODY_ALINE) &&
							alineMatcher.size() >= 3)
						{
							// This is the form where a=attribute:value
							nlohmann::json::json_pointer pkey(
									fmt::format("/b/sdp/{}/{}/{}", blockIndex, key, alineMatcher[1].str()));

							// We may get multiple items for the same "key" such as `a=rtpmap:x` and `a=rtpmap:y`
							// In this case we should start an array
							if (sipm.contains(pkey) && !sipm[pkey].is_array())
							{
								auto						 previousValue = sipm[pkey];
								nlohmann::json::json_pointer pkeyUpOneLevel(fmt::format("/b/sdp/{}/{}", blockIndex, key));
								if (sipm[pkeyUpOneLevel].erase(alineMatcher[1].str()) == 1)
								{
									// Push the first item
									sipm[pkey].push_back(previousValue);
									// Push the current item
									sipm[pkey].push_back(alineMatcher[2].str());
								}
								else
								{
									sip2json_throw<unsupported_contenttype_error>(
											"{}:Failed removing {} from sipmessage.", __func__, string(pkey));
								}
							}
							else if (sipm[pkey].is_array())
								sipm[pkey].push_back(alineMatcher[2].str());
							else
								sipm[pkey] = alineMatcher.length() > 0 ? alineMatcher[2].str() : nullptr;
						}
						else if (!value.empty())
						{
							// This is the form where a=flag
							// We matched a=key without the `:` or the "value" so we should store the value with nullptr
							nlohmann::json::json_pointer pkey(fmt::format("/b/sdp/{}/{}/{}", blockIndex, key, value));
							sipm[pkey] = true;
						}
					}
					else
					{
						nlohmann::json::json_pointer pkey(fmt::format("/b/sdp/{}/{}", blockIndex, key));

						if (key == "c"s)
						{
							match_results<string::iterator> clineMatcher;
							if (regex_search(value.begin(), value.end(), clineMatcher, SIP_PATTERN_BODY_CLINE) &&
								clineMatcher.size() >= 3)
							{
								sipm[pkey] = nlohmann::json {
										{"type"s, clineMatcher[1]}, {"subtype"s, clineMatcher[2]}, {"dn"s, clineMatcher[3]}};
							}
							else
							{
								sipm[pkey] = !value.empty() ? value : nullptr;
							}
						}
						else if (key == "o"s)
						{
							match_results<string::iterator> olineMatcher;
							if (regex_search(value.begin(), value.end(), olineMatcher, SIP_PATTERN_BODY_OLINE) &&
								olineMatcher.size() >= 6)
							{
								sipm[pkey] = nlohmann::json {{"user"s, olineMatcher[1]},
															 {"t1"s, olineMatcher[2]},
															 {"t2"s, olineMatcher[3]},
															 {"type"s, olineMatcher[4]},
															 {"subtype"s, olineMatcher[5]},
															 {"host"s, olineMatcher[6]}};
							}
							else
							{
								sipm[pkey] = !value.empty() ? value : nullptr;
							}
						}
						else if (key.compare("i") == 0)
						{
							// Identity and number and type of call.
							match_results<string::iterator> ilineMatcher;
							if (regex_search(value.begin(), value.end(), ilineMatcher, SIP_PATTERN_BODY_ILINE) &&
								ilineMatcher.size() >= 3)
							{
								sipm[pkey] = nlohmann::json {
										{"name", ilineMatcher[1]}, {"dn", ilineMatcher[2]}, {"type", ilineMatcher[3]}};
							}
							else if (!value.empty())
							{
								sipm[pkey] = value;
							}
							else
							{
								sipm[pkey] = "";
							}
						}
						else if (key == "t"s)
						{
							uint32_t ts = 0, te = 0;
							// timing
							if (sscanf_s(value.c_str(), "%d %d", &ts, &te) > 0)
							{
								sipm[pkey].push_back(ts);
								sipm[pkey].push_back(te);
							}
						}
						else if (!key.empty() && value.empty())
						{
							sipm[pkey] = "";
						}
						else if (!key.empty())
						{
							sipm[pkey] = value;
						}
					}
				}
				// Offset the start to the point after the start-line.
				bufferStart += matcher.length() + ELEM_NEWLINE.size();
			}

			return found;
		}
#pragma endregion

	public:
		/// @brief Given a buffer, parse each message and return a vector of sipmessage objects. If the parseCallback is provided then the return is empty.
		/// @param bufferStart Start of the buffer (modified by call to this method).
		/// @param bufferEnd End of the buffer
		/// @param parseCallback Optional callback which takes a reference to the sipmessage just decoded. If present, the return is empty vector.
		/// @param errorCallback Optional callback to handle the error on the parse.
		/// @return If parseCallback is provided then the return vector is empty otherwise vector of sipmessage decoded within the stream.
		static std::vector<sipmessage>
		parse(std::string::iterator&						  bufferStart,
			  const std::string::iterator&					  bufferEnd,
			  std::optional<std::function<void(sipmessage&)>> parseCallback = {},
			  std::optional<std::function<void(const sip2json_exception&, std::string::iterator&, const std::string::iterator&)>>
					  errorCallback = {}) noexcept
		{
			std::vector<sipmessage> msgs;
			size_t					decodedMessageCount {0};

			while (bufferStart != bufferEnd)
			{
				try
				{
					// If the callback is provided, then we invoke the callback. Nothing is returned to caller.
					if (sipmessage sipm = parseFromBuffer(bufferStart, bufferEnd); parseCallback.has_value())
					{
						decodedMessageCount++;
						sipm["meta"]["parseCountThisBuffer"] = decodedMessageCount;
						parseCallback.value()(sipm);
					}
					else
					{
						decodedMessageCount++;
						sipm["meta"]["parseCountThisBuffer"] = decodedMessageCount;
						// otherwise we push to the vector to return to caller
						msgs.emplace_back(std::move(sipm));
					}
				}
				catch (const invalid_startline_error& e)
				{
					// If the very first one has issues then we should quit.
					if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
					break;
				}
				catch (const unsupported_contenttype_error& e)
				{
					// If the very first one has issues then we should quit.
					if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
					break;
				}
				catch (const incomplete_buffer_for_parse_error& e)
				{
					// If the very first one has issues then we should quit.
					if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
					break;
				}
				catch (const incomplete_buffer_for_header_error& e)
				{
					if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
					break;
				}
				catch (const incomplete_buffer_for_content_error& e)
				{
					if (errorCallback.has_value()) errorCallback.value()(e, bufferStart, bufferEnd);
					break;
				}
				catch (const std::exception& e)
				{
					// Just in case catch-all
					sip2json_exception ex(e);

					if (errorCallback.has_value()) errorCallback.value()(ex, bufferStart, bufferEnd);
					break;
				}
				catch (...)
				{
					// Just in case catch-all
					sip2json_exception ex("Unknown generic error");

					if (errorCallback.has_value()) errorCallback.value()(ex, bufferStart, bufferEnd);
					break;
				}
			}

			return msgs;
		}


		/// @brief De-serialize the *first* SIP message (if present) from the buffer. Repeated calls to this method will extract the remaining messages.
		/// @param bufferStart iterator to the start of the buffer the client expects a SIP message.
		/// @param bufferEnd iterator to the end of the buffer the client expects a SIP message.
		/// @return A sipmessage object containing the document representing the first decoded sipmessage in the buffer.
		static sipmessage parseFromBuffer(std::string::iterator&	   bufferStart,
										  const std::string::iterator& bufferEnd) noexcept(false)
		{
			auto	   previousBufferStart = bufferStart; // save the value so we can reset if we end up with exception.
			sipmessage sipm;
			InvokeOnDestruct timeTaken {[&](long long delta) {
				sipm["meta"]["ttx"]	 = delta;
				sipm["meta"]["pre"]	 = bufferStart - previousBufferStart;
				sipm["meta"]["post"] = bufferEnd - bufferStart;
			}}; // upon destruction, sets the ttx to account for parse time

			if (bufferStart != bufferEnd)
			{
				if (size_t diff = bufferEnd - bufferStart; diff > SIP_SAMPLE_MINIMAL_MESSAGE.length())
				{
					try
					{
						if (auto foundRequest = parseStartLine(sipm, bufferStart, bufferEnd); foundRequest)
						{
							if (auto foundHeaders = parseHeaders(sipm, bufferStart, bufferEnd); foundHeaders)
							{
								if (sipm.getContentType() == CONTENT_TYPE_APP_SDP)
								{
									// It is acceptable in some implementations to declare the Content-Type as application/sdp
									// but provide no actual body. We must not fault this case.
									if (sipm.getContentLength() > 0)
									{
										// Check to make sure that we have sufficient content in the buffer
										// to process the body..
										if (auto availableRemainingBufferSize = bufferEnd - bufferStart;
											availableRemainingBufferSize >= sipm.getContentLength())
										{
											// We must limit the decode to the reported size of the content
											auto bodyEnd = bufferStart;
											bodyEnd += sipm.getContentLength();
											// Decode the SDP
											parseBodySDP(sipm, bufferStart, bodyEnd);
										}
										else
										{
											bufferStart = previousBufferStart;
											sip2json_throw<incomplete_buffer_for_content_error>(
													"{}: Available buffer length:{} < Content-Length:{}",
													__func__,
													availableRemainingBufferSize,
													sipm.getContentLength());
										}
									}
								}
								else if (!sipm.getContentType().empty())
								{
									bufferStart = previousBufferStart;
									sip2json_throw<unsupported_contenttype_error>(
											"{}:Content-Type {} not supported", __func__, sipm.getContentType());
								}
							}
						}
					}
					catch (...)
					{
						// We must reset the buffer to ensure that we can re-parse when there is sufficient buffer
						bufferStart = previousBufferStart;
						// Rethrow
						throw;
					}
				}
				else
				{
					// This will end our scan.
					bufferStart = previousBufferStart;
					sip2json_throw<incomplete_buffer_for_parse_error>("{}:Incomplete Buffer for parse to continue.", __func__);
				}
			}

			return sipm;
		}


		/// @brief Serializes the sipmessage document
		/// @param sipm Source sipmessage
		/// @return Return serialized sipmessage
		static std::string serialize(sipmessage& sipm) noexcept(false)
		{
			using namespace std;

			static const std::string supportedMethods {
					"MESSAGE|INFO|INVITE|ACK|OPTIONS|BYE|CANCEL|REGISTER|SUBSCRIBE|NOTIFY|SIP/2.0"s};
			std::string buffer {};
			std::string contentType {};

			// Assert: non-empty json document
			sip2json_throw_if<empty_message_error>(sipm.size() == 0, "{}:sipm is empty."s, __func__);
			// Assert: non-empty json document; starting with v1.9 we have a meta element for diagnostics; this is to be treated as "empty".
			sip2json_throw_if<empty_message_error>(
					(sipm.contains("meta") && sipm.size() == 1), "{}:sipm is empty (except for meta)."s, __func__);
			// Assert: Method is one of the supported items
			sip2json_throw_if<invalid_document_error>(supportedMethods.find(sipm.getMethod()) == std::string::npos,
													  "{}:Unsupported method:{}"s,
													  __func__,
													  sipm.getMethod());
			// Assert: Header must exist
			sip2json_throw_if<invalid_document_error>(!sipm.contains("h"s), "{}:sipm does not contain `h`eaders."s, __func__);

			if (sipm.isMessageRequest())
			{
				// Request Line
				buffer = fmt::format("{} {} SIP/2.0\r\n"s, sipm.getMethod(), sipm.getUri());
			}
			else if (sipm.isMessageResponse())
			{
				// Status Line
				buffer = fmt::format("SIP/2.0 {} {}\r\n"s, sipm.getStatusCode(), sipm.getReason());
			}
			else
			{
				sip2json_throw<invalid_document_error>(
						"{}:sipm /type is neither `{}` nor `{}`."s, __func__, SIPMessageType::request, SIPMessageType::response);
			}

			// Encode the body first so we can get the content-length properly.
			auto body = serializeSDP(sipm);
			sipm.setHeader("Content-Length"s, body.length());

			// Headers
			if (auto mh = sipm.headers(); mh.size() > 0)
			{
				//TODO: This will not care about the order of the serialized headers. The json library does not care about order.
				for (auto& [key, val] : sipm.headers().items())
				{
					if (contentType.empty() && (key.compare(HF_CONTENT_TYPE) == 0) && val.is_string()) contentType = val;

					if (val.is_null())
					{
						// For null entries, put a blank entry. This is the same as our decode
						buffer += fmt::format("{}: \r\n"s, key);
					}
					else if (val.is_number_unsigned())
					{
						buffer += fmt::format("{}: {}\r\n"s, key, val.get<uint64_t>());
					}
					else if (val.is_number_integer() || val.is_number())
					{
						buffer += fmt::format("{}: {}\r\n"s, key, val.get<int64_t>());
					}
					else if (val.is_number_float())
					{
						buffer += fmt::format("{}: {}\r\n"s, key, val.get<float>());
					}
					else if (val.is_string())
					{
						buffer += fmt::format("{}: {}\r\n"s, key, val);
					}
					else if (val.is_boolean())
					{
						buffer += fmt::format("{}: {}\r\n"s, key, val ? "true" : "false");
					}
					else if (val.is_array())
					{
						// Special handling for arrays.
						// We serialize with the same key and the various values follow.
						for (auto& item : val.items())
						{
							auto iv = item.value();
							buffer += fmt::format("{}: {}\r\n"s, key, iv);
						}
					}
					else
					{
						buffer += fmt::format("{}: {{}}\r\n"s, key, val);
					}
				};

				// End of the message header section
				buffer += ELEM_NEWLINE;
			}

			// Add the body
			buffer += body;

			return buffer;
		}

	private:
		/// @brief Serializes the SDP content
		/// @param sipm sipmessage object
		/// @return string representing the sdp
		static std::string serializeSDP(sipmessage& sipm) noexcept(false)
		{
			using namespace std;

			std::string buffer {};
			auto		contentType = sipm.getContentType();

			// If content-type is not set, then just return regardless of the body element contents.
			if (contentType.empty()) return buffer;

			sip2json_throw_if<invalid_document_error>((contentType.compare(CONTENT_TYPE_APP_SDP) == std::string::npos) &&
															  (contentType.compare(CONTENT_TYPE_TEXT_PLAIN) == std::string::npos),
													  "{}:Unsupported content-type:{}"s,
													  __func__,
													  contentType);

			// Body
			// NOTE: we extract the contentType value during the header serialization.
			if (contentType == CONTENT_TYPE_APP_SDP)
			{
				if (sipm.contains("b"s) && !sipm.body().is_null())
				{
					if (sipm.contains("/b/sdp"_json_pointer))
					{
						// the sdp is stored as an array of objects
						auto sdp = sipm.at("/b/sdp"_json_pointer);
						for (auto& block : sdp)
						{
							// Build each block; order is critical. We do not support session-level attributes (only media-level attributes)
							buffer += fmt::format("v=0\r\no={}\r\ns={}\r\ni={}\r\n"s,
												  serializeSDPelement(block, "o"s),
												  serializeSDPelement(block, "s"s),
												  serializeSDPelement(block, "i"s));
							// Optional..
							if (block.contains("u")) buffer += fmt::format("u={}\r\n"s, serializeSDPelement(block, "u"s));
							// Optional..
							if (block.contains("e")) buffer += fmt::format("e={}\r\n"s, serializeSDPelement(block, "e"s));
							// Optional..
							if (block.contains("p")) buffer += fmt::format("p={}\r\n"s, serializeSDPelement(block, "p"s));
							// Mandatory (typical); No support for session a-lines.
							buffer += fmt::format("c={}\r\nt={}\r\nm={}\r\n"s,
												  serializeSDPelement(block, "c"s),
												  serializeSDPelement(block, "t"s),
												  serializeSDPelement(block, "m"s));
							// Media a-lines
							buffer += serializeSDPelement(block, "a"s);
						}
					}
					else
					{
						sip2json_throw<invalid_document_error>("{}:sipm `b`ody does not have sdp element."s, __func__);
					}
				}
				else
				{
					// This should not be an error; there are live SIP messages where the client sets the Content-Type
					// but also sets the Content-Length to `0` so we should avoid encoding anything.
					//sip2json_throw<invalid_document_error>("{}:sipm does not have b.", __func__);
				}
			}
			else if ((contentType.compare(CONTENT_TYPE_TEXT_PLAIN) == 0) && (sipm.contains("b"s) && sipm.body().is_string()))
			{
				buffer += sipm.body();
			}

			return buffer;
		}


		/// @brief Helper to serialize the SDP element with custom decode
		/// @param sdpBlock The SDP block from the SDP array
		/// @param element The element: o, s, i, c, t, m, a. When returning a= the code builds CRLF terminators.
		/// @return Returns the sdp element as string.
		static std::string serializeSDPelement(nlohmann::json& sdpBlock, const std::string& element)
		{
			using namespace std;

			sip2json_throw_if<missing_required_element>(!sdpBlock.contains("v"s) && !sdpBlock.contains("o"s) &&
																!sdpBlock.contains("s"s) && !sdpBlock.contains("t"s) &&
																!sdpBlock.contains("m"s),
														"{}:Required Element {} not present."s,
														__func__,
														element);
			// If we donot have it then just return..
			if (sdpBlock.contains(element))
			{
				// Continue to build
				if (auto item = sdpBlock.at(element); item.is_object())
				{
					if (element == "a"s)
					{
						std::string ret;

						for (auto& kv : item.items())
						{
							auto v = kv.value();
							if (v.is_array())
							{
								for (auto& i : v.items())
								{
									ret += fmt::format("a={}:{}\r\n"s, kv.key(), i.value());
								}
							}
							else if (v.is_string())
								ret += fmt::format("a={}:{}\r\n"s, kv.key(), v);
							else if (v.is_boolean() && v == true)
								ret += fmt::format("a={}\r\n"s, kv.key());
							else
								ret += fmt::format("a={}\r\n"s, kv.key());
						}

						return ret;
					}
					if (element == "o"s)
					{
						return fmt::format("{} {} {} {} {} {}"s,
										   item.value("user"s, ""s),
										   item.value("t1"s, ""s),
										   item.value("t2"s, ""s),
										   item.value("type"s, ""s),
										   item.value("subtype"s, ""s),
										   item.value("host"s, ""s));
					}
					if (element == "i"s)
					{
						return fmt::format("{} ({}) {}"s, item.value("name"s, ""), item.value("dn"s, ""), item.value("type"s, ""));
					}
					if (element == "c"s)
					{
						return fmt::format("{} {} {}"s, item.value("type"s, ""), item.value("subtype"s, ""), item.value("dn"s, ""));
					}
				}
				else if (item.is_array())
				{
					if (element == "t"s) { return fmt::format("{} {}"s, item[0].get<uint32_t>(), item[1].get<uint32_t>()); }
				}
				else if (item.is_string())
				{
					// In case the parse wasn't able to split properly, it will store it as a string value.
					// Serialize the as-is case.
					return item.get<std::string>();
				}
			}

			return std::string {};
		}
	}; // class sip2json


	// References
	// SIP Messages: https://tools.ietf.org/html/rfc3261#section-7
	// SDP Message format: https://en.wikipedia.org/wiki/Session_Description_Protocol
	// SIP Response Codes: https://en.wikipedia.org/wiki/List_of_SIP_response_codes
	// JSON Library: https://nlohmann.github.io/json/
	// FMT Library : https://fmt.dev/latest/index.html
} // namespace siddiqsoftware

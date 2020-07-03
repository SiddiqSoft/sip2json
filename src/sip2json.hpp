/*
  A SIP Parser for Modern C++
  Version 1.0.0
  https://github.com/siddiqsoftware/sip2json/
  Copyright 2003-2020 Abdelkareem Siddiq.
  All rights reserved.
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

#include "sip2json_response_codes.hpp"
#include "sip2json_utils.hpp"
#include "sipmessage.hpp"

#include "nlohmann/json.hpp"
#include "fmt/chrono.h"

namespace siddiqsoftware
{
#pragma region SIP match patterns
#pragma endregion


	class sip2json
	{
	public:
		static const inline std::string MetaLibName		  = "sip2json";
		static const inline std::string MetaSchemaVersion = "0.1.0";
		static const inline std::string MetaParserVersion = "1.0.0";

		static const inline std::string MessageTypeRequest	= "sip2json.request";
		static const inline std::string MessageTypeResponse = "sip2json.response";


#pragma region SIPMessage helpers
	private:
		/// @brief Creates a basic SIP Message content in json. This method is used by the createRequest and createResponse methods
		/// @param messageType Must be one of MessageTypeRequest or MessageTypeResponse
		/// @return json document with basic sections
		static sipmessage createRawMessage(const std::string& messageType)
		{
			static const std::string userAgent = fmt::format("{}/{}/{}", MetaLibName, MetaParserVersion, MetaSchemaVersion);

			return nlohmann::json {{"type", messageType},
								   {"version", MetaSchemaVersion},
								   {"mb", nullptr},
								   {"mh",
									{{HF_CALLID, nullptr},
									 {HF_TO, nullptr},
									 {HF_FROM, nullptr},
									 {HF_CSEQ, nullptr},
									 {HF_CONTENT_LENGTH, 0},
									 {HF_CONTENT_TYPE, nullptr},
									 {HF_USER_AGENT, userAgent},
									 {HF_MAX_FORWARDS, 0},
									 //{HF_VIA, nullptr},
									 {HF_AUTHORIZATION, nullptr}}}};
		}

	public:
		static sipmessage createRequest(const std::string&		   method,
										const std::string&		   uri,
										const std::string&		   callId = {},
										uint32_t				   cseq	  = 0,
										std::optional<sipmessage> src	  = {})
		{
			auto sipm = src.value_or(createRawMessage(MessageTypeRequest));
			// We must clear these values in case we are updating an existing object.
			if (sipm.contains("sl")) sipm.erase("sl");
			sipm["/type"_json_pointer] = MessageTypeRequest;
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// rl ==> "request-line" (request message type) and sl ==> "status-line" (response message type)
			sipm["/rl/method"_json_pointer]	 = method;
			sipm["/rl/uri"_json_pointer]	 = uri;
			sipm["/rl/version"_json_pointer] = SIPVER_20;
			// message-headers
			if (!callId.empty()) sipm["/mh/Call-ID"_json_pointer] = callId;
			if (cseq > 0) sipm["/mh/CSeq"_json_pointer] = fmt::format("{} {}", cseq, method);
			sipm["/mh/Date"_json_pointer] = getRFC1123();

			return sipm;
		}


		static sipmessage createResponse(uint32_t statusCode, std::optional<sipmessage> src = {})
		{
			auto sipm = src.value_or(createRawMessage(MessageTypeResponse));

			// We must clear these values in case we are updating an existing object.
			if (sipm.contains("rl")) sipm.erase("rl");
			sipm["/type"_json_pointer] = MessageTypeResponse;
			// start-line: may either be a request-line or a status-line
			// request-line: METHOD Request-URI SIP/2.0
			// sl ==> "status-line" (response message type)
			sipm["/sl/status"_json_pointer]	 = statusCode;
			sipm["/sl/reason"_json_pointer]	 = getReasonPhrase(statusCode);
			sipm["/sl/version"_json_pointer] = SIPVER_20;

			return sipm;
		}
#pragma endregion

	public:
		static std::string serialize(nlohmann::json& sipm)
		{
			std::string buffer {};
			std::string contentType {};

			if (sipm.size() == 0) throw std::invalid_argument(fmt::format("{}:sipm is empty.", __func__));


			if (sipm.value("/type"_json_pointer, std::string {}).compare(MessageTypeRequest) == 0)
			{
				// Request Line
				buffer = fmt::format("{} {} SIP/2.0\r\n",
									 sipm.value("/rl/method"_json_pointer, std::string {}),
									 sipm.value("/rl/uri"_json_pointer, std::string {}));
			}
			else if (sipm.value("/type"_json_pointer, std::string {}).compare(MessageTypeResponse) == 0)
			{
				// Status Line
				buffer = fmt::format("SIP/2.0 {} {}\r\n",
									 sipm.value("/sl/status"_json_pointer, 0),
									 sipm.value("/sl/reason"_json_pointer, std::string {}));
			}
			else
			{
				throw std::invalid_argument(
						fmt::format("{}:sipm /type is neither `{}` nor `{}`.", __func__, MessageTypeRequest, MessageTypeResponse));
			}

			// Headers
			auto mh = sipm.at("/mh"_json_pointer);
			if (mh.size() > 0)
			{
				//TODO: This will not care about the order of the serialized headers. The json library does not care about order.
				for (auto& [key, val] : mh.items())
				{
					if (contentType.empty() && (key.compare(HF_CONTENT_TYPE) == 0) && val.is_string()) contentType = val;

					if (val.is_null())
					{ /* do nothing; skip field. */
					}
					else if (val.is_number_unsigned())
					{
						buffer += fmt::format("{}: {}\r\n", key, val.get<uint64_t>());
					}
					else if (val.is_number_integer() || val.is_number())
					{
						buffer += fmt::format("{}: {}\r\n", key, val.get<int64_t>());
					}
					else if (val.is_number_float())
					{
						buffer += fmt::format("{}: {}\r\n", key, val.get<float>());
					}
					else if (val.is_string())
					{
						buffer += fmt::format("{}: {}\r\n", key, val);
					}
					else if (val.is_boolean())
					{
						buffer += fmt::format("{}: {}\r\n", key, val ? "true" : "false");
					}
					else if (val.is_array())
					{
						// Special handling for arrays.
						// We serialize with the same key and the various values follow.
						for (auto& item : val.items())
						{
							auto iv = item.value();
							buffer += fmt::format("{}: {}\r\n", key, iv);
						}
					}
					else
					{
						buffer += fmt::format("{}: {{}}\r\n", key, val);
					}
				};

				// End of the message header section
				buffer += "\r\n";
			}
			else
			{
				throw std::invalid_argument(fmt::format("{}:sipm does not contain mh.", __func__));
			}

			// Body
			// NOTE: we extract the contentType value during the header serialization.
			if (contentType.compare(CONTENT_TYPE_APP_SDP) == 0)
			{
				if (sipm.contains("/mb"_json_pointer))
				{
					if (sipm.contains("/mb/sdp"_json_pointer))
					{
						// the sdp is stored as an array of objects
						auto sdp = sipm.at("/mb/sdp"_json_pointer);
						for (auto& block : sdp)
						{
							buffer += "v=0\r\n"; // always write this out at the start of each SDP element
						}
					}
					else
					{
						throw std::invalid_argument(fmt::format("{}:sipm mb does not have sdp element.", __func__));
					}
				}
				else
				{
					throw std::invalid_argument(fmt::format("{}:sipm does not have mb.", __func__));
				}
			}

			return buffer;
		}

		static bool
		parseStartLine(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			std::match_results<std::string::iterator> matchStartLine;
			bool									  found = false;

			found = std::regex_search(bufferStart, bufferEnd, matchStartLine, SIP_PATTERN_REQUEST_STARTLINE);
			if (!found) found = std::regex_search(bufferStart, bufferEnd, matchStartLine, SIP_PATTERN_RESPONSE_STARTLINE);

			if (found && matchStartLine.size() >= 3)
			{
				if (siddiqsoftware::SIPVER_20.compare(matchStartLine[3]) == 0)
				{
					sipm["type"]					 = MessageTypeRequest;
					sipm["/rl/method"_json_pointer]	 = matchStartLine[1];
					sipm["/rl/uri"_json_pointer]	 = matchStartLine[2];
					sipm["/rl/version"_json_pointer] = matchStartLine[3];
				}
				else
				{
					sipm["type"]						= MessageTypeResponse;
					sipm["/sl/reason"_json_pointer]		= matchStartLine[3];
					sipm["/sl/statusCode"_json_pointer] = std::stoi(matchStartLine[2].str());
					sipm["/sl/version"_json_pointer]	= matchStartLine[1];
				}

				// Offset the start to the point after the start-line. Make sure to skip over any prefix!
				// We may have junk or left-over crud at the start (especially if we're using text files)
				bufferStart += matchStartLine.length() + matchStartLine.prefix().length();
			}
			else
			{
			}

			return found;
		}

	private:
		static bool storeHeaderValue(sipmessage& sipm, const std::string& key, const std::string& value)
		{
			if (key.find(HF_VIA) == 0)
			{
				// Via is an array
				sipm["mh"][HF_VIA].push_back(value);
			}
			else if (_stricmp(key.c_str(), HF_CONTENT_TYPE.c_str()) == 0)
			{
				// Some encoders send Content-type instead of the standard Content-Type; here we need to normalize it.
				sipm["mh"][HF_CONTENT_TYPE] = value;
			}
			else if (key.find(HF_CONTENT_LENGTH) == 0)
			{
				sipm["mh"][key] = std::stoi(value);
			}
			else if (key.find(HF_EXPIRES) == 0)
			{
				sipm["mh"][key] = std::stoi(value);
			}
			else if (value.find("true") == 0)
			{
				sipm["mh"][key] = true;
			}
			else if (value.find("false") == 0)
			{
				sipm["mh"][key] = false;
			}
			else if (value.empty())
			{
				sipm["mh"][key] = nullptr;
			}
			else
			{
				sipm["mh"][key] = value;
			}

			return true;
		}

	private:
		static bool
		parseHeaders2(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			bool done  = false;
			bool found = false;

			auto headerEnd =
					std::search(bufferStart, bufferEnd, ELEM_HEADERSECTIONDELIMITER.begin(), ELEM_HEADERSECTIONDELIMITER.end());

			while (!done)
			{
				// Scan for the first `:`
				auto hsep = std::search(bufferStart, headerEnd, ELEM_PADDEDSEPERATOR.begin(), ELEM_PADDEDSEPERATOR.end());
				if (hsep != headerEnd)
				{
					// Found the separator element.
					// Key is from bufferStart until the separator
					if (std::string key(bufferStart, hsep); !key.empty())
					{
						std::string value {};
						auto		hval = hsep; // Store the location of the value part of the header element.

						// Next, let's look for the end of element
						bufferStart = hsep += ELEM_PADDEDSEPERATOR.size();
					label_recummulate_to_unfold_buffer:
						auto hend = search(hsep, headerEnd, ELEM_NEWLINE.begin(), ELEM_NEWLINE.end());
						if (hend != headerEnd)
						{
							// We found the `\r\n`;
							// Next, check if this is a folded element
							if ((*(hend + ELEM_NEWLINE.size() + 1) == ' ') ||
								((*hend + ELEM_NEWLINE.size() + 1) == '\t')) // peek ahead to see if we have.. folded indicator
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


		static bool
		parseHeaders1(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			std::match_results<std::string::iterator> matcher;
			auto									  headerEnd =
					std::search(bufferStart, bufferEnd, ELEM_HEADERSECTIONDELIMITER.begin(), ELEM_HEADERSECTIONDELIMITER.end());

			if (headerEnd != bufferEnd)
			{
				while (std::regex_search(bufferStart, headerEnd, matcher, SIP_PATTERN_HEADER))
				{
					if (matcher.size() == 3)
					{
						auto key   = matcher[1].str();
						auto value = matcher[2].str();

						if (key.find(HF_VIA) == 0)
						{
							// Via is an array
							sipm["mh"][HF_VIA].push_back(value);
						}
						else if (_stricmp(key.c_str(), HF_CONTENT_TYPE.c_str()) == 0)
						{
							// Some encoders send Content-type instead of the standard Content-Type; here we need to normalize it.
							sipm["mh"][HF_CONTENT_TYPE] = value;
						}
						else if (key.find(HF_CONTENT_LENGTH) == 0)
						{
							sipm["mh"][key] = std::stoi(value);
						}
						else if (key.find(HF_EXPIRES) == 0)
						{
							sipm["mh"][key] = std::stoi(value);
						}
						else if (value.find("true") == 0)
						{
							sipm["mh"][key] = true;
						}
						else if (value.find("false") == 0)
						{
							sipm["mh"][key] = false;
						}
						else if (value.empty())
						{
							sipm["mh"][key] = nullptr;
						}
						else
						{
							sipm["mh"][key] = value;
						}
					}
					// Offset the start to the point after the start-line.
					bufferStart += matcher.length();
				}

				return true;
			}
			else
			{
				throw std::invalid_argument(fmt::format("{} - Buffer missing header-delimiter within range.", __func__));
			}

			return false;
		}


	public:
		static bool
		parseHeaders(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			return parseHeaders2(sipm, bufferStart, bufferEnd);
		}


		static bool
		parseBodySDP(sipmessage& sipm, std::string::iterator& bufferStart, const std::string::iterator& bufferEnd) noexcept(false)
		{
			std::match_results<std::string::iterator> matcher;

			if (bufferStart != bufferEnd)
			{
				uint32_t blockIndex = 0;

				while (std::regex_search(bufferStart, bufferEnd, matcher, SIP_PATTERN_BODY))
				{
					if (matcher.size() == 3)
					{
						auto key   = matcher[1].str();
						auto value = matcher[2].str();

						if (key.compare("a") == 0)
						{
							// attribute lines: https://en.wikipedia.org/wiki/Session_Description_Protocol#Attributes
							//sipm[pkey].push_back(matcher[2].str());
							std::match_results<std::string::iterator> alineMatcher;
							if (std::regex_search(value.begin(), value.end(), alineMatcher, SIP_PATTERN_BODY_ALINE) &&
								alineMatcher.size() >= 3)
							{
								// This is the form where a=attribute:value
								nlohmann::json::json_pointer pkey(
										fmt::format("/mb/sdp/{}/{}/{}", blockIndex, key, alineMatcher[1].str()));
								sipm[pkey] = alineMatcher.length() > 0 ? alineMatcher[2].str() : nullptr;
							}
							else if (!value.empty())
							{
								// This is the form where a=flag
								// We matched a=key without the `:` or the "value" so we should store the value with nullptr
								nlohmann::json::json_pointer pkey(fmt::format("/mb/sdp/{}/{}/{}", blockIndex, key, value));
								sipm[pkey] = true;
							}
						}
						else
						{
							nlohmann::json::json_pointer pkey(fmt::format("/mb/sdp/{}/{}", blockIndex, key));

							if (key.compare("c") == 0)
							{
								std::match_results<std::string::iterator> clineMatcher;
								if (std::regex_search(value.begin(), value.end(), clineMatcher, SIP_PATTERN_BODY_CLINE) &&
									clineMatcher.size() >= 3)
								{
									sipm[pkey] = nlohmann::json {
											{"type", clineMatcher[1]}, {"subtype", clineMatcher[2]}, {"dn", clineMatcher[3]}};
								}
								else
								{
									sipm[pkey] = !value.empty() ? value : nullptr;
								}
							}
							else if (key.compare("o") == 0)
							{
								std::match_results<std::string::iterator> olineMatcher;
								if (std::regex_search(value.begin(), value.end(), olineMatcher, SIP_PATTERN_BODY_OLINE) &&
									olineMatcher.size() >= 6)
								{
									sipm[pkey] = nlohmann::json {{"user", olineMatcher[1]},
																 {"t1", olineMatcher[2]},
																 {"t2", olineMatcher[3]},
																 {"type", olineMatcher[4]},
																 {"subtype", olineMatcher[5]},
																 {"host", olineMatcher[6]}};
								}
								else
								{
									sipm[pkey] = !value.empty() ? value : nullptr;
								}
							}
							else if (key.compare("i") == 0)
							{
								// Identity and number and type of call.
								std::match_results<std::string::iterator> ilineMatcher;
								if (std::regex_search(value.begin(), value.end(), ilineMatcher, SIP_PATTERN_BODY_ILINE) &&
									ilineMatcher.size() >= 3)
								{
									sipm[pkey] = nlohmann::json {
											{"name", ilineMatcher[1]}, {"dn", ilineMatcher[2]}, {"type", ilineMatcher[3]}};
								}
								else
								{
									sipm[pkey] = !value.empty() ? value : nullptr;
								}
							}
							else if (key.compare("t") == 0)
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
								sipm[pkey] = nullptr;
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

				return true;
			}
			else
			{
				throw std::invalid_argument(fmt::format("{} - Buffer missing header-delimiter within range.", __func__));
			}

			return false;
		}

	public:
		static std::string unfoldBuffer(std::string& src)
		{
			static const std::regex SIP_PATTERN_HEADER_FOLDS("[\r\n]{1}[\\s]{1}");

			return std::move(std::regex_replace(src, SIP_PATTERN_HEADER_FOLDS, ""));
		}


		static std::vector<sipmessage> parseAllFromBuffer(std::string::iterator&	   bufferStart,
														  const std::string::iterator& bufferEnd) noexcept(false)
		{
			std::vector<sipmessage> msgs;

			while (bufferStart != bufferEnd)
			{
				msgs.emplace_back(parseFromBuffer(bufferStart, bufferEnd));
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
			sipmessage sipm;

			// Basic assumptions
			// Ensure that the buffer is processable.
			if (bufferStart == bufferEnd) throw std::invalid_argument(fmt::format("{}: bufferStart==bufferEnd", __func__));

			if (bufferStart != bufferEnd)
			{
				if (size_t diff = bufferEnd - bufferStart; diff > SIP_SAMPLE_MINIMAL_MESSAGE.length())
				{
					auto foundRequest = parseStartLine(sipm, bufferStart, bufferEnd);
					if (foundRequest)
					{
						auto foundHeaders = parseHeaders(sipm, bufferStart, bufferEnd);
						if (foundHeaders)
						{
							if (sipm.getContentType().compare(CONTENT_TYPE_APP_SDP) == 0)
							{
								if (sipm.getContentLength() > 0)
								{
									// We must advance the buffer
									bufferStart += ELEM_HEADERSECTIONDELIMITER.length();
									// Decode the SDP
									parseBodySDP(sipm, bufferStart, bufferEnd);
								}
							}
							else if (!sipm.getContentType().empty())
							{
								throw std::exception(
										fmt::format("{}: Content-Type:{} not supported", __func__, sipm.getContentType()).c_str());
							}
						}
						else
						{
							throw std::exception(fmt::format("{}: headers not found", __func__).c_str());
						}
					}
				}
				else
				{
					// Failed; buffer too small
					throw std::length_error(fmt::format("{}: Buffer too small:{} (smaller than reference {})",
														__func__,
														diff,
														SIP_SAMPLE_MINIMAL_MESSAGE.length()));
				}
			}

			return std::move(sipm);
		}
	};


	// References
	// SIP Messages: https://tools.ietf.org/html/rfc3261#section-7
	// SIP Response Codes: https://en.wikipedia.org/wiki/List_of_SIP_response_codes
	// JSON Library: https://nlohmann.github.io/json/
	// FMT Library : https://fmt.dev/latest/index.html
} // namespace siddiqsoftware
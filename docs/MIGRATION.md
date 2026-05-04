# sip2json Migration Guide & Troubleshooting

## Table of Contents

1. [Migration Guide](#migration-guide)
2. [Troubleshooting](#troubleshooting)
3. [Common Issues](#common-issues)
4. [FAQ](#faq)
5. [Performance Tips](#performance-tips)

---

## Migration Guide

### From Version 1.x to 2.0+

The 2.0+ release includes significant improvements while maintaining 100% backward compatibility.

#### What's New

1. **Rule of Five Compliance**
   - Proper copy/move semantics
   - Move operations are `noexcept`
   - Efficient resource management

2. **Const-Correctness**
   - All getter methods are const-qualified
   - Can call getters on const objects
   - Better compiler optimization

3. **Code Quality Improvements**
   - Eliminated goto statements
   - Replaced magic numbers with named constants
   - Consolidated error handling
   - Comprehensive documentation

4. **Enhanced Testing**
   - 142+ tests (up from 64)
   - Rule of Five compliance tests
   - Edge case coverage

#### Migration Steps

**Step 1: Update Include Paths**

```cpp
// Old (still works)
#include "sip2json.hpp"

// New (recommended)
#include "siddiqsoft/sip2json.hpp"
```

**Step 2: Use Const-Correct Code**

```cpp
// Old (still works)
sipmessage& msg = getMsg();
auto method = msg.getMethod();

// New (recommended)
const sipmessage& msg = getMsg();
auto method = msg.getMethod();
```

**Step 3: Leverage Move Semantics**

```cpp
// Old (still works)
std::vector<sipmessage> msgs;
msgs.push_back(msg);

// New (recommended)
msgs.push_back(std::move(msg));
```

**Step 4: Update Error Handling**

```cpp
// Old (still works)
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const std::exception& e) {
    // Handle error
}

// New (recommended)
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const sip2json_exception& e) {
    // Handle SIP-specific errors
} catch (const std::exception& e) {
    // Handle other errors
}
```

#### Backward Compatibility

All existing code continues to work without modification:

```cpp
// ✅ All of these still work
sipmessage msg = getMsg();
auto method = msg.getMethod();
msgs.push_back(msg);
```

---

## Troubleshooting

### Build Issues

#### Issue: "Cannot find header file"

**Symptom**: Compiler error: `fatal error: 'sip2json.hpp' file not found`

**Solution**:
1. Verify include path is correct
2. Check CMakeLists.txt includes sip2json package
3. Ensure header-only library is in include path

```cmake
# CMakeLists.txt
include_directories(/path/to/sip2json/include)
```

#### Issue: "Undefined reference to sip2json"

**Symptom**: Linker error: `undefined reference to 'siddiqsoft::sip2json'`

**Solution**:
- This is a header-only library, no linking required
- Check that you're including the correct header
- Verify C++20 compiler support

#### Issue: "C++20 required"

**Symptom**: Compiler error: `C++20 features required`

**Solution**:
```cmake
# CMakeLists.txt
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

Or with compiler flags:
```bash
g++ -std=c++20 your_file.cpp
clang++ -std=c++20 your_file.cpp
```

### Runtime Issues

#### Issue: "Parse error: SIP Startline not found"

**Symptom**: Exception thrown during parsing

**Causes**:
1. Buffer doesn't contain valid SIP message
2. Buffer is empty or too small
3. Message format is incorrect

**Solution**:
```cpp
// Verify buffer content
if (buffer.empty()) {
    std::cerr << "Buffer is empty" << std::endl;
    return;
}

// Check buffer size
if (buffer.size() < 20) {  // Minimum SIP message size
    std::cerr << "Buffer too small" << std::endl;
    return;
}

// Verify SIP format
if (buffer.find("SIP/2.0") == std::string::npos) {
    std::cerr << "Not a valid SIP message" << std::endl;
    return;
}

// Try parsing
try {
    auto start = buffer.begin();
    auto msg = sip2json::parseFromBuffer(start, buffer.end());
} catch (const sip2json_exception& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

#### Issue: "Incomplete buffer for parse"

**Symptom**: Exception: `incomplete_buffer_for_parse_error`

**Causes**:
1. Buffer contains partial message
2. Message is still being received
3. Buffer size is too small

**Solution**:
```cpp
// Wait for more data
if (buffer.size() < SIP_SAMPLE_MINIMAL_MESSAGE.length()) {
    std::cout << "Waiting for more data..." << std::endl;
    return;  // Try again when more data arrives
}

// Or increase buffer size
buffer.reserve(8192);  // Allocate more space
```

#### Issue: "Content-Length mismatch"

**Symptom**: Exception: `incomplete_buffer_for_content_error`

**Causes**:
1. Content-Length header doesn't match actual body size
2. Body is still being received
3. Message is truncated

**Solution**:
```cpp
// Check Content-Length
auto contentLength = msg.getContentLength();
auto availableSize = bufferEnd - bufferStart;

if (availableSize < contentLength) {
    std::cout << "Waiting for body: " 
              << contentLength - availableSize 
              << " bytes remaining" << std::endl;
    return;  // Wait for more data
}
```

#### Issue: "Unsupported content type"

**Symptom**: Exception: `unsupported_contenttype_error`

**Causes**:
1. Content-Type is not "application/sdp" or "text/plain"
2. Custom content types not supported

**Solution**:
```cpp
// Check supported content types
auto contentType = msg.getContentType();
if (contentType == "application/sdp" || 
    contentType == "text/plain" ||
    contentType.empty()) {
    // Supported
} else {
    std::cerr << "Unsupported content type: " << contentType << std::endl;
}
```

### Memory Issues

#### Issue: "Memory leak detected"

**Symptom**: Memory leak warnings from tools like Valgrind

**Solution**:
1. Ensure proper use of move semantics
2. Use smart pointers (unique_ptr, shared_ptr)
3. Avoid manual memory management

```cpp
// ✅ Good: Stack allocation
sipmessage msg("INVITE", "sip:test@example.com");

// ✅ Good: Smart pointer
auto msg_ptr = std::make_unique<sipmessage>("INVITE", "sip:test@example.com");

// ❌ Avoid: Manual allocation
auto msg_ptr = new sipmessage("INVITE", "sip:test@example.com");
delete msg_ptr;  // Easy to forget
```

#### Issue: "Buffer overflow"

**Symptom**: Segmentation fault or memory corruption

**Solution**:
1. Always check buffer bounds
2. Use iterators instead of raw pointers
3. Validate input size

```cpp
// ✅ Good: Use iterators
auto start = buffer.begin();
auto end = buffer.end();
auto msg = sip2json::parseFromBuffer(start, end);

// ❌ Avoid: Raw pointers
const char* ptr = buffer.c_str();
// Risk of buffer overflow
```

---

## Common Issues

### Issue 1: CRLF vs LF Line Endings

**Problem**: Parser fails with different line endings

**Solution**:
```cpp
// The parser handles both CRLF and LF
// But ensure consistency in your messages

// ✅ Good: CRLF (standard for SIP)
std::string msg = "INVITE sip:test@example.com SIP/2.0\r\n"
                  "Via: SIP/2.0/TCP example.com\r\n"
                  "\r\n";

// ⚠️ Also works: LF only
std::string msg = "INVITE sip:test@example.com SIP/2.0\n"
                  "Via: SIP/2.0/TCP example.com\n"
                  "\n";
```

### Issue 2: Header Folding

**Problem**: Multi-line headers not parsed correctly

**Solution**:
```cpp
// ✅ Good: Properly folded headers (RFC 2822)
std::string msg = "INVITE sip:test@example.com SIP/2.0\r\n"
                  "Via: SIP/2.0/TCP example.com,\r\n"
                  " SIP/2.0/TCP backup.com\r\n"
                  "\r\n";

// The parser automatically handles header folding
auto msg_obj = sip2json::parseFromBuffer(start, end);
```

### Issue 3: Multiple Headers with Same Name

**Problem**: Multiple Via headers not handled correctly

**Solution**:
```cpp
// ✅ Good: Multiple headers stored as array
sipmessage msg("INVITE", "sip:test@example.com");
msg.headers()["Via"].push_back("SIP/2.0/TCP example.com");
msg.headers()["Via"].push_back("SIP/2.0/TCP backup.com");

// Access as array
for (const auto& via : msg.headers()["Via"]) {
    std::cout << "Via: " << via << std::endl;
}
```

### Issue 4: SDP Body Parsing

**Problem**: SDP body not parsed correctly

**Solution**:
```cpp
// ✅ Good: Set Content-Type before body
sipmessage msg("INVITE", "sip:test@example.com");
msg.setHeader("Content-Type", "application/sdp");
msg.setHeader("Content-Length", 0);

// Body is automatically parsed if Content-Type is application/sdp
// Access SDP elements
auto version = msg.getBodyElement<int>("/sdp/0/v", 0);
```

### Issue 5: Empty Messages

**Problem**: Serialization fails for empty messages

**Solution**:
```cpp
// ✅ Good: Ensure message has required fields
sipmessage msg("INVITE", "sip:test@example.com");
msg.setHeader("From", "sip:caller@example.com");
msg.setHeader("To", "sip:user@example.com");
msg.setHeader("Via", "SIP/2.0/TCP example.com");

// Now serialization will work
auto sip_string = sip2json::serialize(msg);

// ❌ Avoid: Empty message
sipmessage empty_msg;
auto sip_string = sip2json::serialize(empty_msg);  // Throws exception
```

---

## FAQ

### Q: Is the library thread-safe?

**A**: No. The library is not thread-safe. Each thread should have its own sipmessage instances. Use synchronization primitives (mutex, etc.) when sharing messages between threads.

### Q: Can I use this library in production?

**A**: Yes. The library is production-ready with 142+ tests and comprehensive error handling. It's used in production systems.

### Q: What's the performance overhead?

**A**: Minimal. The library uses compile-time regex patterns (CTRE) and move semantics for efficiency. Parsing is O(n) where n is message size.

### Q: Can I extend the library?

**A**: Yes. The library is designed to be extended. You can:
1. Subclass sipmessage for custom behavior
2. Add custom headers
3. Implement custom SDP parsing

### Q: How do I handle custom headers?

**A**: Custom headers are automatically supported:

```cpp
msg.setHeader("X-Custom-Header", "value");
auto value = msg.getHeader<std::string>("X-Custom-Header");
```

### Q: Can I parse multiple messages from a buffer?

**A**: Yes. Use either `parse()` or `parseAsync()`:

```cpp
// Batch parsing
auto messages = sip2json::parse(start, end);

// Async parsing
sip2json::parseAsync(buffer, [](sipmessage&& msg) {
    // Process each message
});
```

### Q: How do I handle parsing errors?

**A**: Use try-catch blocks:

```cpp
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const sip2json_exception& e) {
    std::cerr << "Parse error: " << e.what() << std::endl;
}
```

### Q: Can I modify a parsed message?

**A**: Yes. sipmessage is mutable:

```cpp
auto msg = sip2json::parseFromBuffer(start, end);
msg.setHeader("X-Modified", "true");
auto modified = sip2json::serialize(msg);
```

### Q: What's the minimum buffer size?

**A**: The minimum valid SIP message is about 20 bytes. The library defines `SIP_SAMPLE_MINIMAL_MESSAGE` for reference.

### Q: How do I debug parsing issues?

**A**: Enable detailed error messages:

```cpp
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const sip2json_exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    std::cerr << "Type: " << typeid(e).name() << std::endl;
}
```

---

## Performance Tips

### 1. Pre-allocate Buffer Space

```cpp
// ✅ Good: Pre-allocate
std::string buffer;
buffer.reserve(8192);  // Allocate 8KB upfront

// ❌ Avoid: Dynamic allocation
std::string buffer;  // Allocates as needed
```

### 2. Use Move Semantics

```cpp
// ✅ Good: Move
std::vector<sipmessage> msgs;
msgs.push_back(std::move(msg));

// ❌ Avoid: Copy
msgs.push_back(msg);
```

### 3. Use Async Parsing for Streams

```cpp
// ✅ Good: Async for streaming
sip2json::parseAsync(buffer, callback);

// ⚠️ Less efficient: Batch parsing
auto msgs = sip2json::parse(start, end);
```

### 4. Reuse Buffers

```cpp
// ✅ Good: Reuse buffer
std::string buffer;
buffer.reserve(8192);

while (hasMoreData()) {
    buffer.clear();
    readData(buffer);
    sip2json::parseAsync(buffer, callback);
}

// ❌ Avoid: Create new buffer each time
while (hasMoreData()) {
    std::string buffer;  // New allocation each iteration
    readData(buffer);
    sip2json::parseAsync(buffer, callback);
}
```

### 5. Use Const References

```cpp
// ✅ Good: Const reference
void processMessage(const sipmessage& msg) {
    auto method = msg.getMethod();
}

// ❌ Avoid: Non-const reference
void processMessage(sipmessage& msg) {
    auto method = msg.getMethod();
}
```

### 6. Cache Frequently Accessed Values

```cpp
// ✅ Good: Cache values
const auto& method = msg.getMethod();
const auto& uri = msg.getUri();
// Use method and uri multiple times

// ❌ Avoid: Repeated lookups
if (msg.getMethod() == "INVITE") { ... }
if (msg.getMethod() == "INVITE") { ... }  // Redundant lookup
```

---

## Getting Help

### Resources

1. **API Documentation**: See `docs/API.md`
2. **Architecture Guide**: See `docs/ARCHITECTURE.md`
3. **GitHub Issues**: Report bugs at https://github.com/siddiqsoftware/sip2json/issues
4. **Examples**: Check `tests/` directory for usage examples

### Reporting Issues

When reporting issues, include:
1. Minimal reproducible example
2. Error message and stack trace
3. Compiler and version
4. Operating system
5. Input data (if possible)

---

**Last Updated**: 2024  
**Version**: 2.0+  
**Status**: Production Ready ✅

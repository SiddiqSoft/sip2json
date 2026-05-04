# sip2json Architecture & Design Patterns

## Table of Contents

1. [Architecture Overview](#architecture-overview)
2. [Design Patterns](#design-patterns)
3. [Class Hierarchy](#class-hierarchy)
4. [Data Flow](#data-flow)
5. [Memory Management](#memory-management)
6. [Error Handling](#error-handling)
7. [Performance Optimization](#performance-optimization)

---

## Architecture Overview

### High-Level Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Application Layer                    │
│                  (User Application Code)                │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                   sip2json API Layer                    │
│  ┌──────────────────────────────────────────────────┐   │
│  │  sipmessage (JSON-based SIP Message Container)   │   │
│  │  - Request/Response representation               │   │
│  │  - Header management                             │   │
│  │  - Body (SDP) management                         │   │
│  └──────────────────────────────────────────────────┘   │
│  ┌──────────────────────────────────────────────────┐   │
│  │  sip2json (Factory/Utility Class)                │   │
│  │  - Parse SIP messages                            │   │
│  │  - Serialize to SIP format                       │   │
│  │  - Async streaming support                       │   │
│  └──────────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│              Parsing & Serialization Layer              │
│  ┌──────────────────────────────────────────────────┐   │
│  │  CTRE (Compile-Time Regular Expressions)         │   │
│  │  - SIP start line parsing                        │   │
│  │  - Header parsing                                │   │
│  │  - SDP body parsing                              │   │
│  └──────────────────────────────────────────────────┘   │
└────────────────────┬────────────────────────────────────┘
                     │
┌────────────────────▼────────────────────────────────────┐
│                  JSON Layer                             │
│  ┌──────────────────────────────────────────────────┐   │
│  │  nlohmann::json (JSON Library)                   │   │
│  │  - JSON document representation                  │   │
│  │  - JSON pointer support                          │   │
│  │  - Serialization/Deserialization                 │   │
│  └──────────────────────────────────────────────────┘   │
└─────────────────────────────────────────────────────────┘
```

### Component Responsibilities

| Component | Responsibility |
|-----------|-----------------|
| **sipmessage** | Represents a single SIP message with JSON storage |
| **sip2json** | Factory methods for parsing and serialization |
| **CTRE** | Compile-time regex patterns for parsing |
| **nlohmann::json** | Underlying JSON document storage |

---

## Design Patterns

### 1. Factory Pattern

The `sip2json` class implements the Factory pattern for creating sipmessage objects:

```cpp
// Factory methods
static sipmessage parseFromBuffer(...);
static std::vector<sipmessage> parse(...);
static std::string serialize(sipmessage& sipm);
```

**Benefits**:
- Encapsulates object creation logic
- Provides consistent interface for parsing
- Allows for future implementation changes

### 2. Builder Pattern

The `sipmessage` class supports the Builder pattern through method chaining:

```cpp
msg.setHeader("From", "sip:test@example.com")
   .setHeader("To", "sip:user@example.com")
   .setHeader("User-Agent", "MyApp/1.0");
```

**Benefits**:
- Fluent API for object construction
- Readable and maintainable code
- Flexible object creation

### 3. Strategy Pattern

Different parsing strategies for different message types:

```cpp
// Strategy 1: Parse request
if (SIPVER_20 == g3) {
    // Parse as request
}
// Strategy 2: Parse response
else if (SIPVER_20 == g1) {
    // Parse as response
}
```

**Benefits**:
- Flexible parsing based on message type
- Easy to extend for new message types

### 4. Template Method Pattern

The parsing process follows a template:

1. Parse start line
2. Parse headers
3. Parse body (if present)
4. Return sipmessage

```cpp
// Template method in parseFromBuffer()
parseStartLine(sipm, bufferStart, bufferEnd);
parseHeaders(sipm, bufferStart, bufferEnd);
parseBodySDP(sipm, bufferStart, bodyEnd);
```

**Benefits**:
- Consistent parsing flow
- Easy to maintain and extend
- Clear separation of concerns

### 5. Adapter Pattern

The `sipmessage` class adapts nlohmann::json for SIP-specific operations:

```cpp
class sipmessage : public nlohmann::json {
    // Adapts JSON to SIP domain
    auto getMethod() const;
    auto getUri() const;
    auto getStatusCode() const;
};
```

**Benefits**:
- Leverages existing JSON library
- Provides SIP-specific interface
- Maintains JSON compatibility

### 6. Rule of Five Pattern

Proper implementation of special member functions:

```cpp
class sipmessage {
public:
    sipmessage();                                    // Default constructor
    sipmessage(const sipmessage&);                   // Copy constructor
    sipmessage(sipmessage&&) noexcept;               // Move constructor
    sipmessage& operator=(const sipmessage&);        // Copy assignment
    sipmessage& operator=(sipmessage&&) noexcept;    // Move assignment
    ~sipmessage() = default;                         // Destructor
};
```

**Benefits**:
- Proper resource management
- Efficient move semantics
- Exception safety

---

## Class Hierarchy

### sipmessage Class Hierarchy

```
nlohmann::json
    │
    └── sipmessage
        ├── Constructors (6 variants)
        ├── Assignment Operators (4 variants)
        ├── Header Methods
        │   ├── setHeader()
        │   ├── getHeader()
        │   └── headers()
        ├── Getter Methods (const-qualified)
        │   ├── getMethod()
        │   ├── getUri()
        │   ├── getStatusCode()
        │   ├── getReason()
        │   ├── getCallID()
        │   ├── getUserAgent()
        │   ├── getContentLength()
        │   ├── getContentType()
        │   ├── getExpires()
        │   ├── isMessageRequest()
        │   ├── isMessageResponse()
        │   └── hasBody()
        └── Body Methods
            ├── body()
            └── getBodyElement()
```

### sip2json Class Structure

```
sip2json (Static Utility Class)
├── Static Methods
│   ├── parseAsync()
│   ├── parse()
│   ├── parseFromBuffer()
│   └── serialize()
├── Private Methods
│   ├── parseStartLine()
│   ├── parseHeaders()
│   ├── parseBodySDP()
│   ├── storeHeaderValue()
│   ├── serializeSDP()
│   └── serializeSDPelement()
└── Private Constants
    ├── TYPICAL_SIP_MESSAGE_SIZE
    └── METADATA_ONLY_SIZE
```

---

## Data Flow

### Parsing Flow

```
Input Buffer (SIP Message String)
    │
    ▼
┌─────────────────────────────────┐
│  parseFromBuffer()              │
│  ├─ parseStartLine()            │
│  │  └─ Extract method/URI/status│
│  ├─ parseHeaders()              │
│  │  └─ Extract all headers      │
│  └─ parseBodySDP()              │
│     └─ Extract SDP elements     │
└─────────────────────────────────┘
    │
    ▼
sipmessage (JSON-based)
    │
    ├─ "s" (start line)
    ├─ "h" (headers)
    ├─ "b" (body/SDP)
    └─ "meta" (metadata)
```

### Serialization Flow

```
sipmessage (JSON-based)
    │
    ▼
┌─────────────────────────────────┐
│  serialize()                    │
│  ├─ Build start line            │
│  ├─ Build headers               │
│  ├─ serializeSDP()              │
│  │  └─ Build SDP body           │
│  └─ Combine all parts           │
└─────────────────────────────────┘
    │
    ▼
Output String (SIP Message)
```

### Async Parsing Flow

```
Input Buffer
    │
    ▼
┌─────────────────────────────────┐
│  parseAsync()                   │
│  while (bufferStart != end) {   │
│    ├─ parseFromBuffer()         │
│    ├─ Invoke parseCallback()    │
│    └─ Advance bufferStart       │
│  }                              │
└─────────────────────────────────┘
    │
    ├─ Callback 1: sipmessage
    ├─ Callback 2: sipmessage
    └─ Callback N: sipmessage
```

---

## Memory Management

### Stack vs Heap Allocation

```cpp
// ✅ Stack allocation (preferred)
sipmessage msg("INVITE", "sip:test@example.com");

// ⚠️ Heap allocation (when necessary)
auto msg_ptr = std::make_unique<sipmessage>("INVITE", "sip:test@example.com");
```

### Move Semantics

```cpp
// Efficient move (no copy)
std::vector<sipmessage> messages;
messages.push_back(std::move(msg));  // O(1) operation

// Inefficient copy (if move not used)
messages.push_back(msg);  // O(n) operation
```

### Resource Ownership

```cpp
// Callback receives ownership via move
sip2json::parseAsync(buffer, [](sipmessage&& msg) {
                        // msg is moved, not copied
                        // Process message
                    });
```

### Memory Optimization

1. **Named Constants**: Reduce magic numbers
   ```cpp
   static constexpr size_t TYPICAL_SIP_MESSAGE_SIZE = 3 * 1024;
   buffer.reserve(TYPICAL_SIP_MESSAGE_SIZE);
   ```

2. **String Views**: Avoid unnecessary copies
   ```cpp
   auto g1 = matchStartLine.get<1>().to_view();  // string_view, not string
   ```

3. **JSON Pointers**: Efficient element access
   ```cpp
   auto version = msg.getBodyElement<int>("/sdp/0/v", 0);
   ```

---

## Error Handling

### Exception Hierarchy

```
std::exception
    │
    └── sip2json_exception
        ├── invalid_startline_error
        ├── incomplete_buffer_for_parse_error
        ├── incomplete_buffer_for_header_error
        ├── incomplete_buffer_for_content_error
        ├── unsupported_contenttype_error
        ├── empty_message_error
        ├── invalid_document_error
        └── missing_required_element
```

### Error Handling Strategy

```cpp
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const sip2json_exception& e) {
    // Handle SIP-specific errors
    std::cerr << "SIP Error: " << e.what() << std::endl;
} catch (const std::exception& e) {
    // Handle other errors
    std::cerr << "Error: " << e.what() << std::endl;
}
```

### Exception Safety Guarantees

- **Strong Guarantee**: Operations either succeed completely or have no effect
- **No-Throw Guarantee**: Move operations are `noexcept`
- **Basic Guarantee**: Parsing may partially modify buffer on error

---

## Performance Optimization

### Compile-Time Optimizations

1. **CTRE (Compile-Time Regular Expressions)**
   - Regex patterns compiled at compile-time
   - Zero runtime overhead for pattern compilation
   - Type-safe pattern matching

2. **Constexpr Constants**
   ```cpp
   static constexpr size_t TYPICAL_SIP_MESSAGE_SIZE = 3 * 1024;
   ```

3. **Inline Methods**
   - Header-only library enables aggressive inlining
   - Compiler can optimize across translation units

### Runtime Optimizations

1. **Buffer Reuse**
   ```cpp
   buffer.reserve(TYPICAL_SIP_MESSAGE_SIZE);  // Pre-allocate
   ```

2. **Iterator-Based Parsing**
   - No string copies during parsing
   - Direct buffer access via iterators

3. **Move Semantics**
   - Efficient transfer of ownership
   - No unnecessary copies

4. **JSON Pointer Caching**
   - Efficient element access
   - Avoids repeated lookups

### Benchmarking

Performance characteristics:
- **Parsing**: O(n) where n = message size
- **Serialization**: O(n) where n = message size
- **Header Access**: O(1) average case
- **Body Element Access**: O(log n) via JSON pointer

---

## Best Practices for Library Users

### 1. Use Const-Correct Code

```cpp
// ✅ Good
const sipmessage& msg = getMsg();
auto method = msg.getMethod();

// ❌ Avoid
sipmessage& msg = getMsg();
auto method = msg.getMethod();
```

### 2. Leverage Move Semantics

```cpp
// ✅ Good
std::vector<sipmessage> msgs;
msgs.push_back(std::move(msg));

// ❌ Avoid
msgs.push_back(msg);  // Unnecessary copy
```

### 3. Use Async Parsing for Streams

```cpp
// ✅ Good for streaming
sip2json::parseAsync(buffer, callback);

// ⚠️ Alternative for batch
auto msgs = sip2json::parse(start, end);
```

### 4. Handle Exceptions Properly

```cpp
// ✅ Good
try {
    auto msg = sip2json::parseFromBuffer(start, end);
} catch (const sip2json_exception& e) {
    // Handle error
}

// ❌ Avoid
auto msg = sip2json::parseFromBuffer(start, end);  // Unhandled exception
```

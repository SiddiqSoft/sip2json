#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <filesystem>
#include <iomanip>

#include "siddiqsoft/sip2json.hpp"

namespace fs = std::filesystem;

struct SampleFile {
    std::string filename;
    std::string content;
    size_t size_bytes;
};

int main(int argc, char** argv)
{
    std::string samples_dir = "samples";
    if (argc > 1) {
        samples_dir = argv[1];
    } else if (!fs::exists(samples_dir)) {
        auto cwd = fs::current_path();
        std::vector<fs::path> candidates = {
            cwd / "tests" / "validation" / "samples",
            cwd / "samples",
            cwd.parent_path() / "tests" / "validation" / "samples",
            cwd.parent_path() / "samples",
            cwd.parent_path().parent_path() / "tests" / "validation" / "samples",
            cwd.parent_path().parent_path() / "samples",
            cwd.parent_path().parent_path().parent_path() / "tests" / "validation" / "samples",
            cwd.parent_path().parent_path().parent_path() / "samples",
            cwd.parent_path().parent_path().parent_path().parent_path() / "tests" / "validation" / "samples",
            cwd.parent_path().parent_path().parent_path().parent_path() / "samples"
        };
        for (const auto& cand : candidates) {
            if (fs::exists(cand) && fs::is_directory(cand)) {
                samples_dir = cand.string();
                break;
            }
        }
    }

    if (!fs::exists(samples_dir)) {
        std::cerr << "Error: samples directory not found at " << samples_dir << std::endl;
        return 1;
    }

    std::vector<SampleFile> sample_files;
    size_t total_sample_bytes = 0;

    for (const auto& entry : fs::directory_iterator(samples_dir)) {
        if (entry.is_regular_file() && entry.path().extension() == ".sip") {
            std::ifstream file(entry.path(), std::ios::binary);
            if (file) {
                std::string content((std::istreambuf_iterator<char>(file)),
                                     std::istreambuf_iterator<char>());
                total_sample_bytes += content.size();
                sample_files.push_back({entry.path().filename().string(), content, content.size()});
            }
        }
    }

    if (sample_files.empty()) {
        std::cerr << "No .sip files found in " << samples_dir << std::endl;
        return 1;
    }

    std::cout << "================================================================================" << std::endl;
    std::cout << "  SIP2JSON Benchmark Harness" << std::endl;
    std::cout << "  Loaded " << sample_files.size() << " sample files (" 
              << (total_sample_bytes / 1024.0) << " KB total)" << std::endl;
    std::cout << "================================================================================" << std::endl;

    constexpr int ITERATIONS = 300;
    size_t total_messages_parsed = 0;
    size_t total_bytes_processed = 0;

    // Warmup pass
    for (const auto& sf : sample_files) {
        try {
            std::string buffer = sf.content;
            auto bs = buffer.begin();
            auto result = siddiqsoft::sip2json::parse(bs, buffer.end());
            (void)result;
        } catch (...) {}
    }

    // Benchmark Pass 1: Multi-message stream parsing (sip2json::parse)
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < ITERATIONS; ++iter) {
        for (const auto& sf : sample_files) {
            try {
                std::string buffer = sf.content;
                auto bs = buffer.begin();
                auto messages = siddiqsoft::sip2json::parse(bs, buffer.end());
                total_messages_parsed += messages.size();
                total_bytes_processed += sf.size_bytes;
            } catch (...) {}
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    double total_time_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();
    double total_time_sec = total_time_ms / 1000.0;

    double msg_per_sec = total_messages_parsed / total_time_sec;
    double mb_per_sec = (total_bytes_processed / (1024.0 * 1024.0)) / total_time_sec;
    double avg_us_per_msg = (total_time_ms * 1000.0) / total_messages_parsed;

    std::cout << std::fixed << std::setprecision(2);
    std::cout << "\n[BENCHMARK RESULTS - sip2json::parse (Stream)]" << std::endl;
    std::cout << "  Total Iterations    : " << ITERATIONS << std::endl;
    std::cout << "  Total Execution Time: " << total_time_ms << " ms (" << total_time_sec << " s)" << std::endl;
    std::cout << "  Total Messages      : " << total_messages_parsed << std::endl;
    std::cout << "  Total Data Processed: " << (total_bytes_processed / (1024.0 * 1024.0)) << " MB" << std::endl;
    std::cout << "  Throughput          : " << msg_per_sec << " msg/sec" << std::endl;
    std::cout << "  Data Bandwidth      : " << mb_per_sec << " MB/sec" << std::endl;
    std::cout << "  Avg Latency/Msg     : " << avg_us_per_msg << " us/msg" << std::endl;

    // Benchmark Pass 2: Single-Message parseFromBuffer
    size_t single_messages_parsed = 0;
    size_t single_bytes_processed = 0;

    std::vector<SampleFile> valid_single_files;
    for (const auto& sf : sample_files) {
        try {
            std::string buffer = sf.content;
            auto bs = buffer.begin();
            auto msg = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
            if (msg.isMessageRequest() || msg.isMessageResponse()) {
                valid_single_files.push_back(sf);
            }
        } catch (...) {}
    }

    constexpr int SINGLE_ITERATIONS = 1000;
    auto start_single = std::chrono::high_resolution_clock::now();

    for (int iter = 0; iter < SINGLE_ITERATIONS; ++iter) {
        for (const auto& sf : valid_single_files) {
            try {
                std::string buffer = sf.content;
                auto bs = buffer.begin();
                auto msg = siddiqsoft::sip2json::parseFromBuffer(bs, buffer.end());
                (void)msg;
                single_messages_parsed++;
                single_bytes_processed += sf.size_bytes;
            } catch (...) {}
        }
    }

    auto end_single = std::chrono::high_resolution_clock::now();
    double single_time_ms = std::chrono::duration<double, std::milli>(end_single - start_single).count();
    double single_time_sec = single_time_ms / 1000.0;
    double single_msg_per_sec = single_messages_parsed / single_time_sec;
    double single_avg_us = (single_time_ms * 1000.0) / single_messages_parsed;

    std::cout << "\n[BENCHMARK RESULTS - sip2json::parseFromBuffer (Single)]" << std::endl;
    std::cout << "  Valid Single Files  : " << valid_single_files.size() << std::endl;
    std::cout << "  Single Iterations   : " << SINGLE_ITERATIONS << std::endl;
    std::cout << "  Total Execution Time: " << single_time_ms << " ms" << std::endl;
    std::cout << "  Total Messages      : " << single_messages_parsed << std::endl;
    std::cout << "  Throughput          : " << single_msg_per_sec << " msg/sec" << std::endl;
    std::cout << "  Avg Latency/Msg     : " << single_avg_us << " us/msg" << std::endl;
    std::cout << "================================================================================" << std::endl;

    return 0;
}

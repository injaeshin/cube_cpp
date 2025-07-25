// #ifndef RING_BUFFER_TESTS_HPP
// #define RING_BUFFER_TESTS_HPP
//
// #include "catch_amalgamated.hpp"
// #include "core/buffer/ring_buffer.hpp"
//
// #include <cstring>
//
// using namespace buffer;
//
// TEST_CASE("RingBuffer 기본 생성 및 초기 상태", "[RingBuffer]") {
//     RingBuffer buffer(10);
//
//     SECTION("초기 상태 확인") {
//         REQUIRE(buffer.ReadableBytes() == 0);
//         REQUIRE(buffer.WritableBytes() == 10);
//     }
// }
//
// TEST_CASE("RingBuffer 기본 쓰기/읽기 테스트", "[RingBuffer]") {
//     RingBuffer buffer(10);
//
//     SECTION("기본 쓰기 작업") {
//         uint8_t data[] = {1, 2, 3, 4, 5};
//         size_t written = buffer.Write(data, 5);
//
//         REQUIRE(written == 5);
//         REQUIRE(buffer.ReadableBytes() == 5);
//         REQUIRE(buffer.WritableBytes() == 5);
//     }
//
//     SECTION("기본 읽기 작업") {
//         uint8_t write_data[] = {1, 2, 3, 4, 5};
//         buffer.Write(write_data, 5);
//
//         uint8_t read_data[5];
//         size_t read = buffer.Read(read_data, 5);
//
//         REQUIRE(read == 5);
//         REQUIRE(std::memcmp(write_data, read_data, 5) == 0);
//         REQUIRE(buffer.ReadableBytes() == 0);
//         REQUIRE(buffer.WritableBytes() == 10);
//     }
// }
//
// TEST_CASE("RingBuffer 용량 초과 테스트", "[RingBuffer]") {
//     RingBuffer buffer(5);
//
//     SECTION("용량 초과 쓰기") {
//         uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
//         size_t written = buffer.Write(data, 8);
//
//         REQUIRE(written == 5);  // 용량만큼만 쓰여짐
//         REQUIRE(buffer.ReadableBytes() == 5);
//         REQUIRE(buffer.WritableBytes() == 0);
//     }
//
//     SECTION("빈 버퍼에서 읽기") {
//         uint8_t data[5];
//         size_t read = buffer.Read(data, 5);
//
//         REQUIRE(read == 0);  // 읽을 데이터가 없음
//     }
// }
//
// TEST_CASE("RingBuffer 랩어라운드 테스트", "[RingBuffer]") {
//     RingBuffer buffer(5);
//
//     SECTION("부분 읽기 후 추가 쓰기") {
//         // 처음에 5바이트 쓰기
//         uint8_t data1[] = {1, 2, 3, 4, 5};
//         buffer.Write(data1, 5);
//
//         // 3바이트 읽기
//         uint8_t read_data[3];
//         buffer.Read(read_data, 3);
//
//         REQUIRE(buffer.ReadableBytes() == 2);
//         REQUIRE(buffer.WritableBytes() == 3);
//
//         // 3바이트 추가 쓰기 (랩어라운드 발생)
//         uint8_t data2[] = {6, 7, 8};
//         size_t written = buffer.Write(data2, 3);
//
//         REQUIRE(written == 3);
//         REQUIRE(buffer.ReadableBytes() == 5);
//         REQUIRE(buffer.WritableBytes() == 0);
//
//         // 모든 데이터 읽기 확인
//         uint8_t final_data[5];
//         buffer.Read(final_data, 5);
//
//         uint8_t expected[] = {4, 5, 6, 7, 8};
//         REQUIRE(std::memcmp(final_data, expected, 5) == 0);
//     }
// }
//
// TEST_CASE("RingBuffer Peek 기능 테스트", "[RingBuffer]") {
//     RingBuffer buffer(10);
//     uint8_t data[] = {1, 2, 3, 4, 5};
//     buffer.Write(data, 5);
//
//     SECTION("Peek으로 데이터 미리보기") {
//         uint8_t peek_data[3];
//         size_t peeked = buffer.Peek(peek_data, 3);
//
//         REQUIRE(peeked == 3);
//         REQUIRE(peek_data[0] == 1);
//         REQUIRE(peek_data[1] == 2);
//         REQUIRE(peek_data[2] == 3);
//
//         // Peek 후에도 버퍼 상태는 변하지 않음
//         REQUIRE(buffer.ReadableBytes() == 5);
//         REQUIRE(buffer.WritableBytes() == 5);
//     }
//
//     SECTION("Peek 후 실제 읽기") {
//         uint8_t peek_data[3];
//         buffer.Peek(peek_data, 3);
//
//         uint8_t read_data[5];
//         size_t read = buffer.Read(read_data, 5);
//
//         REQUIRE(read == 5);
//         REQUIRE(std::memcmp(data, read_data, 5) == 0);
//     }
// }
//
// TEST_CASE("RingBuffer Advance 기능 테스트", "[RingBuffer]") {
//     RingBuffer buffer(10);
//     uint8_t data[] = {1, 2, 3, 4, 5};
//     buffer.Write(data, 5);
//
//     SECTION("AdvanceRead 테스트") {
//         buffer.AdvanceRead(2);
//
//         REQUIRE(buffer.ReadableBytes() == 3);
//         REQUIRE(buffer.WritableBytes() == 7);
//
//         uint8_t read_data[3];
//         buffer.Read(read_data, 3);
//
//         uint8_t expected[] = {3, 4, 5};
//         REQUIRE(std::memcmp(read_data, expected, 3) == 0);
//     }
//
//     SECTION("AdvanceWrite 테스트") {
//         buffer.AdvanceWrite(3);  // 쓰기 포지션을 3만큼 이동
//
//         REQUIRE(buffer.ReadableBytes() == 8);  // 5 + 3
//         REQUIRE(buffer.WritableBytes() == 2);  // 10 - 8
//     }
// }
//
// TEST_CASE("RingBuffer 포인터 기능 테스트", "[RingBuffer]") {
//     RingBuffer buffer(10);
//
//     SECTION("WriterPtr과 ReadPtr 테스트") {
//         uint8_t* write_ptr = buffer.WriterPtr();
//         const uint8_t* read_ptr = buffer.ReadPtr();
//
//         REQUIRE(write_ptr != nullptr);
//         REQUIRE(read_ptr != nullptr);
//
//         // 직접 포인터를 통해 데이터 쓰기
//         *write_ptr = 100;
//         buffer.AdvanceWrite(1);
//
//         REQUIRE(buffer.ReadableBytes() == 1);
//         REQUIRE(*buffer.ReadPtr() == 100);
//     }
// }
//
// TEST_CASE("RingBuffer 경계 조건 테스트", "[RingBuffer]") {
//     SECTION("크기 1인 버퍼") {
//         RingBuffer buffer(1);
//
//         uint8_t data = 42;
//         REQUIRE(buffer.Write(&data, 1) == 1);
//         REQUIRE(buffer.ReadableBytes() == 1);
//         REQUIRE(buffer.WritableBytes() == 0);
//
//         uint8_t read_data;
//         REQUIRE(buffer.Read(&read_data, 1) == 1);
//         REQUIRE(read_data == 42);
//     }
//
//     SECTION("여러 번의 작은 쓰기/읽기") {
//         RingBuffer buffer(3);
//
//         for (int i = 0; i < 10; ++i) {
//             uint8_t data = static_cast<uint8_t>(i);
//             buffer.Write(&data, 1);
//
//             uint8_t read_data;
//             buffer.Read(&read_data, 1);
//
//             REQUIRE(read_data == i);
//         }
//     }
// }
//
// #endif  // RING_BUFFER_TESTS_HPP

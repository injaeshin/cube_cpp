// #ifndef PACKET_DISPATCHER_TESTS_HPP
// #define PACKET_DISPATCHER_TESTS_HPP
//
// #include "core/core_pch.hpp"
// #include "catch_amalgamated.hpp"
// #include "core/dispatcher/packet_dispatcher.hpp"
// #include "core/session/session_base.hpp"
// #include "protocol/packet_header.hpp"
//
// using namespace protocol;
// using namespace dispatcher;
// using namespace session;
//
// // 테스트용 모의 세션 클래스
// class MockSession : public session::ISession {
// public:
//     MockSession() = default;
//
//     // ISession 인터페이스 구현
//     void OnError(const std::string& message) override { last_error = message; }
//
//     void OnConnected() override { connected = true; }
//
//     void OnDisconnected() override { disconnected = true; }
//
//     void OnReceived(const uint8_t* data, size_t length) override {
//         received_data.assign(data, data + length);
//         received_count++;
//     }
//
//     void Send(const std::vector<uint8_t>& data) override {
//         // sent_data.assign(data.begin(), data.end());
//         // sent_count++;
//     }
//
//     // 테스트용 상태 변수
//     std::vector<uint8_t> received_data;
//     int received_count = 0;
//     std::string last_error;
//     bool connected = false;
//     bool disconnected = false;
// };
//
// TEST_CASE("PacketDispatcher 기본 생성 및 초기 상태", "[PacketDispatcher]") {
//     PacketDispatcher dispatcher;
//
//     SECTION("기본 생성") {
//         // 기본 생성자로 생성 가능한지 확인
//         REQUIRE(true);  // 예외가 발생하지 않으면 성공
//     }
// }
//
// TEST_CASE("PacketDispatcher 핸들러 등록 테스트", "[PacketDispatcher]") {
//     PacketDispatcher dispatcher;
//
//     SECTION("핸들러 등록") {
//         bool handler_called = false;
//         auto test_handler = [&handler_called](session::ISession& session, const uint8_t* data,
//                                               size_t length) { handler_called = true; };
//
//         dispatcher.RegisterHandler(PacketID::REQ_ECHO, test_handler);
//         // 등록이 성공적으로 완료되면 예외가 발생하지 않음
//         REQUIRE(true);
//     }
//
//     SECTION("동일한 패킷 타입에 대한 핸들러 재등록") {
//         int call_count = 0;
//         auto handler1 = [&call_count](session::ISession& session, const uint8_t* data,
//                                       size_t length) { call_count = 1; };
//
//         auto handler2 = [&call_count](session::ISession& session, const uint8_t* data,
//                                       size_t length) { call_count = 2; };
//
//         dispatcher.RegisterHandler(PacketID::REQ_ECHO, handler1);
//         dispatcher.RegisterHandler(PacketID::REQ_ECHO, handler2);  // 재등록
//
//         // 재등록이 성공적으로 완료되면 예외가 발생하지 않음
//         REQUIRE(true);
//     }
// }
//
// TEST_CASE("PacketDispatcher 알려진 패킷 디스패치 테스트", "[PacketDispatcher]") {
//     PacketDispatcher dispatcher;
//     auto mock_session = std::make_shared<MockSession>();
//
//     SECTION("등록된 핸들러로 패킷 디스패치") {
//         bool handler_called = false;
//         std::vector<uint8_t> received_payload;
//         size_t received_length = 0;
//
//         auto test_handler = [&handler_called, &received_payload, &received_length](
//                                 session::ISession& session, const uint8_t* data,
//                                 size_t length) {
//             handler_called = true;
//             received_payload.assign(data, data + length);
//             received_length = length;
//         };
//
//         dispatcher.RegisterHandler(PacketID::REQ_ECHO, test_handler);
//
//         // 테스트 패킷 생성
//         PacketHeader header;
//         header.length = PACKET_HEADER_SIZE + 4;  // 헤더 + 4바이트 페이로드
//         header.id = static_cast<uint16_t>(PacketID::REQ_ECHO);
//
//         std::vector<uint8_t> packet_data;
//         packet_data.resize(PACKET_HEADER_SIZE + 4);
//
//         // 헤더 복사
//         std::memcpy(packet_data.data(), &header, PACKET_HEADER_SIZE);
//
//         // 페이로드 설정
//         uint8_t payload[] = {1, 2, 3, 4};
//         std::memcpy(packet_data.data() + PACKET_HEADER_SIZE, payload, 4);
//
//         // 디스패치 실행
//         dispatcher.Dispatch(*mock_session, packet_data.data(), packet_data.size());
//
//         REQUIRE(handler_called == true);
//         REQUIRE(received_length == 4);
//         REQUIRE(received_payload.size() == 4);
//         REQUIRE(received_payload[0] == 1);
//         REQUIRE(received_payload[1] == 2);
//         REQUIRE(received_payload[2] == 3);
//         REQUIRE(received_payload[3] == 4);
//     }
//
//     SECTION("여러 핸들러 등록 및 디스패치") {
//         bool echo_handler_called = false;
//         bool response_handler_called = false;
//
//         auto echo_handler = [&echo_handler_called](session::ISession& session,
//                                                    const uint8_t* data,
//                                                    size_t length) { echo_handler_called = true; };
//
//         auto response_handler = [&response_handler_called](session::ISession& session,
//                                                            const uint8_t* data, size_t length) {
//             response_handler_called = true;
//         };
//
//         dispatcher.RegisterHandler(PacketID::REQ_ECHO, echo_handler);
//         dispatcher.RegisterHandler(PacketID::RES_ECHO, response_handler);
//
//         // REQ_ECHO 패킷 생성 및 디스패치
//         PacketHeader header1;
//         header1.length = PACKET_HEADER_SIZE + 2;
//         header1.id = static_cast<uint16_t>(PacketID::REQ_ECHO);
//
//         std::vector<uint8_t> packet1;
//         packet1.resize(PACKET_HEADER_SIZE + 2);
//         std::memcpy(packet1.data(), &header1, PACKET_HEADER_SIZE);
//
//         dispatcher.Dispatch(*mock_session, packet1.data(), packet1.size());
//
//         REQUIRE(echo_handler_called == true);
//         REQUIRE(response_handler_called == false);
//
//         // RES_ECHO 패킷 생성 및 디스패치
//         PacketHeader header2;
//         header2.length = PACKET_HEADER_SIZE + 3;
//         header2.id = static_cast<uint16_t>(PacketID::RES_ECHO);
//
//         std::vector<uint8_t> packet2;
//         packet2.resize(PACKET_HEADER_SIZE + 3);
//         std::memcpy(packet2.data(), &header2, PACKET_HEADER_SIZE);
//
//         dispatcher.Dispatch(*mock_session, packet2.data(), packet2.size());
//
//         REQUIRE(echo_handler_called == true);
//         REQUIRE(response_handler_called == true);
//     }
// }
//
// TEST_CASE("PacketDispatcher 알 수 없는 패킷 처리 테스트", "[PacketDispatcher]") {
//     PacketDispatcher dispatcher;
//     auto mock_session = std::make_shared<MockSession>();
//
//     SECTION("등록되지 않은 패킷 타입 처리") {
//         // 핸들러를 등록하지 않은 상태에서 알 수 없는 패킷 디스패치
//         PacketHeader header;
//         header.length = PACKET_HEADER_SIZE + 2;
//         header.id = 9999;  // 등록되지 않은 패킷 ID
//
//         std::vector<uint8_t> packet_data;
//         packet_data.resize(PACKET_HEADER_SIZE + 2);
//         std::memcpy(packet_data.data(), &header, PACKET_HEADER_SIZE);
//
//         // 예외가 발생하지 않고 정상적으로 처리되어야 함
//         REQUIRE_NOTHROW(dispatcher.Dispatch(*mock_session, packet_data.data(), packet_data.size()));
//     }
// }
//
// TEST_CASE("PacketDispatcher 패킷 헤더 파싱 테스트", "[PacketDispatcher]") {
//     PacketDispatcher dispatcher;
//     auto mock_session = std::make_shared<MockSession>();
//
//     SECTION("헤더 크기보다 작은 데이터 처리") {
//         uint8_t small_data[] = {0, 1};  // 헤더 크기(4바이트)보다 작음
//
//         // 예외가 발생하지 않고 정상적으로 처리되어야 함
//         REQUIRE_NOTHROW(dispatcher.Dispatch(*mock_session, small_data, 2));
//     }
//
//     SECTION("잘못된 패킷 크기 처리") {
//         PacketHeader header;
//         header.length = 2;  // 헤더 크기보다 작은 패킷 크기
//         header.id = static_cast<uint16_t>(PacketID::REQ_ECHO);
//
//         std::vector<uint8_t> packet_data;
//         packet_data.resize(PACKET_HEADER_SIZE);
//         std::memcpy(packet_data.data(), &header, PACKET_HEADER_SIZE);
//
//         // 예외가 발생하지 않고 정상적으로 처리되어야 함
//         REQUIRE_NOTHROW(dispatcher.Dispatch(*mock_session, packet_data.data(), packet_data.size()));
//     }
//
//     SECTION("빈 데이터 처리") {
//         // 빈 데이터로 디스패치 시도
//         REQUIRE_NOTHROW(dispatcher.Dispatch(*mock_session, nullptr, 0));
//     }
// }
//
// TEST_CASE("PacketDispatcher 세션 전달 테스트", "[PacketDispatcher]") {
//     PacketDispatcher dispatcher;
//     auto mock_session = std::make_shared<MockSession>();
//
//     SECTION("핸들러에서 세션 객체 접근") {
//         session::ISession* received_session = nullptr;
//
//         auto test_handler = [&received_session](session::ISession& session,
//                                                 const uint8_t* data,
//                                                 size_t length) { received_session = &session; };
//
//         dispatcher.RegisterHandler(PacketID::REQ_ECHO, test_handler);
//
//         PacketHeader header;
//         header.length = PACKET_HEADER_SIZE + 1;
//         header.id = static_cast<uint16_t>(PacketID::REQ_ECHO);
//
//         std::vector<uint8_t> packet_data;
//         packet_data.resize(PACKET_HEADER_SIZE + 1);
//         std::memcpy(packet_data.data(), &header, PACKET_HEADER_SIZE);
//
//         dispatcher.Dispatch(*mock_session, packet_data.data(), packet_data.size());
//
//         REQUIRE(received_session != nullptr);
//         REQUIRE(received_session == mock_session.get());
//     }
// }
//
// TEST_CASE("PacketDispatcher 경계 조건 테스트", "[PacketDispatcher]") {
//     PacketDispatcher dispatcher;
//     auto mock_session = std::make_shared<MockSession>();
//
//     SECTION("최대 크기 패킷 처리") {
//         bool handler_called = false;
//         auto test_handler = [&handler_called](session::ISession& session, const uint8_t* data,
//                                               size_t length) { handler_called = true; };
//
//         dispatcher.RegisterHandler(PacketID::REQ_ECHO, test_handler);
//
//         // 최대 크기의 패킷 생성 (헤더 + 큰 페이로드)
//         PacketHeader header;
//         header.length = PACKET_HEADER_SIZE + 1000;
//         header.id = static_cast<uint16_t>(PacketID::REQ_ECHO);
//
//         std::vector<uint8_t> packet_data;
//         packet_data.resize(PACKET_HEADER_SIZE + 1000);
//         std::memcpy(packet_data.data(), &header, PACKET_HEADER_SIZE);
//
//         // 페이로드에 패턴 데이터 설정
//         for (size_t i = 0; i < 1000; ++i) {
//             packet_data[PACKET_HEADER_SIZE + i] = static_cast<uint8_t>(i % 256);
//         }
//
//         REQUIRE_NOTHROW(dispatcher.Dispatch(*mock_session, packet_data.data(), packet_data.size()));
//         REQUIRE(handler_called == true);
//     }
//
//     SECTION("여러 핸들러 동시 등록") {
//         std::vector<bool> handler_calls(10, false);
//
//         // 여러 핸들러 등록
//         for (int i = 0; i < 10; ++i) {
//             auto handler = [&handler_calls, i](session::ISession& session,
//                                                const uint8_t* data,
//                                                size_t length) { handler_calls[i] = true; };
//
//             dispatcher.RegisterHandler(static_cast<PacketID>(1000 + i), handler);
//         }
//
//         // 각 핸들러 테스트
//         for (int i = 0; i < 10; ++i) {
//             PacketHeader header;
//             header.length = PACKET_HEADER_SIZE + 1;
//             header.id = 1000 + i;
//
//             std::vector<uint8_t> packet_data;
//             packet_data.resize(PACKET_HEADER_SIZE + 1);
//             std::memcpy(packet_data.data(), &header, PACKET_HEADER_SIZE);
//
//             dispatcher.Dispatch(*mock_session, packet_data.data(), packet_data.size());
//
//             REQUIRE(handler_calls[i] == true);
//         }
//     }
// }
//
// #endif  // PACKET_DISPATCHER_TESTS_HPP

#include "game/handlers/echo_handler.hpp"
#include "protocol/generated/echo_generated.h"

void game::EchoHandler::Handle(core::ISession& session, const protocol::EchoRequest* request) {
    // FlatBuffers로 응답 생성
    flatbuffers::FlatBufferBuilder builder;
    auto messageOffset = builder.CreateString(request->message()->str());
    auto echoResponse = protocol::CreateEchoResponse(builder, messageOffset);
    builder.Finish(echoResponse);

    // 세션으로 응답 전송
    // session.Send(builder.GetBufferPointer(), builder.GetSize());
}
# Action RPG 게임 서버 개발 Overview

## 프로젝트 목표
- **목적**: 포트폴리오용 Action RPG 게임 서버 개발
- **클라이언트**: Unreal Engine 5 연동
- **핵심 기능**: 실시간 액션 게임에 필요한 동기화 시스템 구현

## 사용 라이브러리
- **asio** (header-only): 비동기 네트워크 I/O
- **flatbuffers**: 직렬화/역직렬화
- **cmake**: 빌드 시스템
- **concurrentQueue**: 스레드 안전 큐
- **spdlog**: 로깅
- **date.h**: 시간 처리
- **catch2**: 테스팅 프레임워크

---

## 개발 단계별 모듈 구성

### 🟢 1단계: 기본 서버 인프라 (완료)

#### Core 모듈
- **Session Management**
  - `session_base.cpp/hpp`: 기본 세션 클래스
  - `session_manager.cpp/hpp`: 세션 관리자
  - `isession.hpp`: 세션 인터페이스

- **Network Layer**
  - `tcp_acceptor.cpp/hpp`: TCP 연결 수락기

- **Buffer System** 
  - `ring_buffer.hpp`: 링 버퍼 구현

- **Packet Dispatcher**
  - `packet_dispatcher.hpp`: 패킷 처리 분산
  - `packet_execute_queue.cpp/hpp`: 패킷 실행 큐
  - `ipacket_dispatcher.hpp`: 디스패처 인터페이스

- **Common Utilities**
  - `service_registry.hpp`: 서비스 등록/관리
  - `queue_adapter.hpp`: 큐 어댑터

#### Protocol 모듈
- **Packet System**
  - `packet.hpp`: 기본 패킷 구조
  - FlatBuffers 스키마 통합

#### Game 모듈
- **Handler System**
  - `handler_manager.cpp/hpp`: 핸들러 관리
  - `handler_factory.cpp/hpp`: 핸들러 팩토리
  - `echo_handler.cpp/hpp`: 에코 핸들러 (테스트용)

---

### 🔵 2단계: 메모리 최적화 (진행 예정)

#### 🔴 우선순위: Buffer Pool 시스템
- **목표**: 패킷 버퍼의 메모리 풀 관리
- **구현 위치**: `src/core/buffer/`
- **주요 컴포넌트**:
  - `buffer_pool.hpp/cpp`: 메모리 풀 관리자
  - `pooled_buffer.hpp/cpp`: 풀에서 관리되는 버퍼 클래스
  - `buffer_allocator.hpp/cpp`: 커스텀 할당자

**예상 구현 내용**:
```cpp
// 메모리 풀 기반 버퍼 관리
class BufferPool {
    // 고정 크기 블록들의 메모리 풀
    // 빠른 할당/해제를 위한 free list
    // 스레드 안전성 보장
};

class PooledBuffer {
    // RAII 기반 버퍼 관리
    // 자동 반환 메커니즘
};
```

#### Object Pool 시스템
- **목표**: 게임 오브젝트 메모리 풀
- **구현 위치**: `src/core/pool/`
- **주요 컴포넌트**:
  - `object_pool.hpp/cpp`: 범용 오브젝트 풀
  - `pool_allocator.hpp/cpp`: 풀 기반 할당자

---

### 🔵 3단계: 액션 게임 핵심 시스템

#### Tick Manager
- **목표**: 게임 루프 및 시뮬레이션 틱 관리
- **구현 위치**: `src/game/tick/`
- **주요 컴포넌트**:
  - `tick_manager.hpp/cpp`: 틱 관리자
  - `fixed_timestep.hpp/cpp`: 고정 시간 스텝
  - `variable_timestep.hpp/cpp`: 가변 시간 스텝

#### Lag Compensation (지연 보상)
- **목표**: 네트워크 지연에 대한 보상 처리
- **구현 위치**: `src/game/compensation/`
- **주요 컴포넌트**:
  - `lag_compensator.hpp/cpp`: 지연 보상 처리기
  - `snapshot_history.hpp/cpp`: 게임 상태 스냅샷 히스토리
  - `rollback_system.hpp/cpp`: 롤백 시스템

#### Interpolation System (보간 처리)
- **목표**: 부드러운 움직임을 위한 보간
- **구현 위치**: `src/game/interpolation/`
- **주요 컴포넌트**:
  - `interpolator.hpp/cpp`: 보간 처리기
  - `position_interpolator.hpp/cpp`: 위치 보간
  - `rotation_interpolator.hpp/cpp`: 회전 보간

---

### 🔵 4단계: 게임 로직 시스템

#### Entity Component System (ECS)
- **목표**: 효율적인 게임 오브젝트 관리
- **구현 위치**: `src/game/ecs/`
- **주요 컴포넌트**:
  - `entity_manager.hpp/cpp`: 엔터티 관리
  - `component_system.hpp/cpp`: 컴포넌트 시스템
  - `transform_component.hpp/cpp`: 변환 컴포넌트

#### Physics Integration
- **목표**: 물리 엔진 통합 (간단한 AABB 충돌)
- **구현 위치**: `src/game/physics/`
- **주요 컴포넌트**:
  - `collision_detector.hpp/cpp`: 충돌 감지
  - `physics_world.hpp/cpp`: 물리 월드

#### Game State Management
- **목표**: 게임 상태 관리
- **구현 위치**: `src/game/state/`
- **주요 컴포넌트**:
  - `game_state.hpp/cpp`: 게임 상태
  - `state_synchronizer.hpp/cpp`: 상태 동기화

---

### 🔵 5단계: 고급 네트워킹

#### Reliable UDP
- **목표**: 안정적인 UDP 통신
- **구현 위치**: `src/core/network/`
- **주요 컴포넌트**:
  - `reliable_udp.hpp/cpp`: 신뢰성 있는 UDP
  - `packet_sequencer.hpp/cpp`: 패킷 순서 관리

#### Delta Compression
- **목표**: 대역폭 최적화를 위한 델타 압축
- **구현 위치**: `src/protocol/compression/`
- **주요 컴포넌트**:
  - `delta_compressor.hpp/cpp`: 델타 압축기
  - `state_differ.hpp/cpp`: 상태 차이 계산

---

### 🔵 6단계: 테스트 및 최적화

#### Performance Testing
- **목표**: 성능 테스트 및 프로파일링
- **구현 위치**: `tests/performance/`
- **주요 컴포넌트**:
  - 로드 테스트
  - 메모리 사용량 측정
  - 네트워크 처리량 테스트

#### UE5 클라이언트 연동
- **목표**: Unreal Engine 5와의 실제 연동 테스트
- **구현 내용**:
  - 프로토콜 호환성 확인
  - 실시간 통신 테스트
  - 동기화 정확성 검증

---

## 다음 단계: Buffer Pool 구현

### 구현 계획
1. **기본 메모리 풀 구조 설계**
   - 고정 크기 블록 풀
   - Free list 관리
   - 스레드 안전성

2. **패킷 버퍼와 통합**
   - 기존 `ring_buffer`와 연동
   - 패킷 송수신 최적화

3. **성능 측정 및 튜닝**
   - 메모리 할당/해제 성능 비교
   - 메모리 단편화 방지

### 예상 구현 일정
- **설계 및 인터페이스 정의**: 1일
- **기본 구현**: 2-3일  
- **테스트 및 통합**: 1-2일
- **성능 최적화**: 1일

---

## 성공 지표
- ✅ 1000+ 동시 연결 처리
- ✅ 60 TPS (Ticks Per Second) 안정적 처리
- ✅ 50ms 이하 지연 보상 처리
- ✅ 메모리 누수 없음
- ✅ UE5 클라이언트 실시간 연동 
#ifndef RING_BUFFER_HPP
#define RING_BUFFER_HPP

namespace core {

class RingBuffer {
public:
    explicit RingBuffer(const size_t capacity = 2048)
        : _buffer(capacity), _capacity(capacity), _readPos(0), _writePos(0), _isFull(false) {}
    ~RingBuffer() = default;

    size_t Write(const uint8_t* src, const size_t length) {
        size_t written = 0;
        while (written < length && WritableBytes() > 0) {
            _buffer[_writePos] = src[written];
            _writePos = (_writePos + 1) % _capacity;
            ++written;

            if (_writePos == _readPos) {
                _isFull = true;
            }
        }
        return written;
    }

    size_t Read(uint8_t* dest, const size_t length) {
        size_t read = 0;
        while (read < length && ReadableBytes() > 0) {
            dest[read] = _buffer[_readPos];
            _readPos = (_readPos + 1) % _capacity;
            ++read;
            _isFull = false;
        }
        return read;
    }

    size_t Peek(uint8_t* dest, const size_t length) const {
        size_t peek = 0;
        size_t pos = _readPos;
        while (peek < length && ReadableBytes() > 0) {
            dest[peek] = _buffer[pos];
            pos = (pos + 1) % _capacity;
            ++peek;

            if (pos == _writePos && _isFull) break;
        }
        return peek;
    }

    void AdvanceRead(const size_t length) {
        _readPos = (_readPos + length) % _capacity;
        _isFull = false;
    }

    void AdvanceWrite(const size_t length) {
        _writePos = (_writePos + length) % _capacity;
        if (_writePos == _readPos) {
            _isFull = true;
        }
    }

    size_t ReadableBytes() const {
        if (_isFull) {
            return _capacity;
        }

        if (_writePos >= _readPos) {
            return _writePos - _readPos;
        }

        return _capacity - _readPos + _writePos;
    }

    uint8_t* WriterPtr() { return &_buffer[_writePos]; }
    const uint8_t* ReadPtr() const { return &_buffer[_readPos]; }

    size_t WritableBytes() const { return _capacity - ReadableBytes(); }

private:
    std::vector<uint8_t> _buffer;
    const size_t _capacity;
    size_t _readPos;
    size_t _writePos;
    bool _isFull;
};

}
#endif
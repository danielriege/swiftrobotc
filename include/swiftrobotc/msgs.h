#ifndef msgs
#define msgs

#define ARRAY_SIZE_SIZE 4

// base_msgs
#define UINT8ARRAY_MSG      0x0001
#define UINT16ARRAY_MSG     0x0002
#define UINT32ARRAY_MSG     0x0003
#define INT8ARRAY_MSG       0x0004
#define INT16ARRAY_MSG      0x0005
#define INT32ARRAY_MSG      0x0006
#define FLOATARRAY_MSG      0x0007
// internal_msgs
#define UPDATE_MSG          0x0101

struct Message {
    uint32_t serialize(char* data) {}
    static Message deserialize(char* data) {return Message();}
    static const uint16_t type = 0;
};

namespace base_msgs{

struct UInt8Array: Message {
    uint32_t size;
    std::vector<uint8_t> data;
    
    uint32_t serialize(char* dat) {
        size = data.size() * sizeof(uint8_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
        return size + sizeof(uint32_t);
    }
    
    static UInt8Array deserialize(char* dat) {
        UInt8Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<uint8_t> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    static const uint16_t type = UINT8ARRAY_MSG;
};

struct UInt16Array: Message {
    uint32_t size;
    std::vector<uint16_t> data;
    
    uint32_t serialize(char* dat) {
        size = data.size() * sizeof(uint16_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
        return size + sizeof(uint32_t);
    }
    
    static UInt16Array deserialize(char* dat) {
        UInt16Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<uint16_t> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    static const uint16_t type = UINT16ARRAY_MSG;
};

struct UInt32Array: Message {
    uint32_t size;
    std::vector<uint32_t> data;
    
    uint32_t serialize(char* dat) {
        size = data.size() * sizeof(uint32_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
        return size + sizeof(uint32_t);
    }
    
    static UInt32Array deserialize(char* dat) {
        UInt32Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<uint32_t> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    static const uint16_t type = UINT32ARRAY_MSG;
};

struct Int8Array: Message {
    uint32_t size;
    std::vector<int8_t> data;
    
    uint32_t serialize(char* dat) {
        size = data.size() * sizeof(int8_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
        return size + sizeof(uint32_t);
    }
    
    static Int8Array deserialize(char* dat) {
        Int8Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<int8_t> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    static const uint16_t type = INT8ARRAY_MSG;
};

struct Int16Array: Message {
    uint32_t size;
    std::vector<int16_t> data;
    
    uint32_t serialize(char* dat) {
        size = data.size() * sizeof(int16_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
        return size + sizeof(uint32_t);
    }
    
    static Int16Array deserialize(char* dat) {
        Int16Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<int16_t> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    static const uint16_t type = INT16ARRAY_MSG;
};

struct Int32Array: Message {
    uint32_t size;
    std::vector<int32_t> data;
    
    uint32_t serialize(char* dat) {
        size = data.size() * sizeof(int32_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
        return size + sizeof(uint32_t);
    }
    
    static Int32Array deserialize(char* dat) {
        Int32Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<int32_t> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    static const uint16_t type = INT32ARRAY_MSG;
};

struct FloatArray: Message {
    uint32_t size;
    std::vector<float> data;
    
    uint32_t serialize(char* dat) {
        size = data.size() * sizeof(float);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
        return size + sizeof(uint32_t);
    }
    
    static FloatArray deserialize(char* dat) {
        FloatArray msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<float> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    static const uint16_t type = FLOATARRAY_MSG;
};

}

namespace internal_msgs {

enum status_t {
    ATTACHED = 0,
    DETACHED = 1,
    CONNECTED = 2
};

struct UpdateMsg: Message {
    
    uint8_t deviceID;
    enum status_t status;
    
    uint32_t serialize(char* dat) {
        memcpy(dat, &deviceID, sizeof(uint8_t));
        memcpy(dat + sizeof(uint8_t), &status, sizeof(uint8_t));
        return 2 * sizeof(uint8_t);
    }
    
    static UpdateMsg deserialize(char* dat) {
        UpdateMsg msg;
        msg.deviceID = (uint8_t)dat[0];
        msg.status = (enum status_t)dat[1];
        return msg;
    }
    
    static const uint16_t type = UPDATE_MSG;
};

}

#endif

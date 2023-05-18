#ifndef msgs
#define msgs

#include <stdint.h>
#include <map>

#define ARRAY_SIZE_SIZE 4

// base_msg
#define UINT8ARRAY_MSG      0x0001
#define UINT16ARRAY_MSG     0x0002
#define UINT32ARRAY_MSG     0x0003
#define INT8ARRAY_MSG       0x0004
#define INT16ARRAY_MSG      0x0005
#define INT32ARRAY_MSG      0x0006
#define FLOATARRAY_MSG      0x0007
// internal_msg
#define UPDATE_MSG          0x0101
// sensor_msg
#define IMAGE_MSG           0x0201
#define IMU_MSG             0x0202
// control_msg
#define DRIVE_MSG           0x0301
// nav_msg
#define ODOMETRY_MSG        0x0401

struct Message {
    void serialize(char* data) {}
    static Message deserialize(char* data) {return Message();}
    uint32_t getSize();
    static const uint16_t type = 0;
};

namespace base_msg{

struct UInt8Array: Message {
    uint32_t size;
    std::vector<uint8_t> data;
    
    void serialize(char* dat) {
        size = data.size() * sizeof(uint8_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
    }
    
    static UInt8Array deserialize(char* dat) {
        UInt8Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        std::vector<uint8_t> data(dat + sizeof(uint32_t), dat + sizeof(uint32_t) + msg.size);
        msg.data = data;
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(uint8_t);
    }
    
    static const uint16_t type = UINT8ARRAY_MSG;
};

struct UInt16Array: Message {
    uint32_t size;
    std::vector<uint16_t> data;
    
    void serialize(char* dat) {
        size = data.size() * sizeof(uint16_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
    }
    
    static UInt16Array deserialize(char* dat) {
        UInt16Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        msg.data = std::vector<uint16_t>(msg.size/sizeof(uint32_t));
        memcpy(&msg.data[0], dat+sizeof(uint32_t), msg.size);
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(uint16_t);
    }
    
    static const uint16_t type = UINT16ARRAY_MSG;
};

struct UInt32Array: Message {
    uint32_t size;
    std::vector<uint32_t> data;
    
    void serialize(char* dat) {
        size = data.size() * sizeof(uint32_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
    }
    
    static UInt32Array deserialize(char* dat) {
        UInt32Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        msg.data = std::vector<uint32_t>(msg.size/sizeof(uint32_t));
        memcpy(&msg.data[0], dat+sizeof(uint32_t), msg.size);
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(uint32_t);
    }
    
    static const uint16_t type = UINT32ARRAY_MSG;
};

struct Int8Array: Message {
    uint32_t size;
    std::vector<int8_t> data;
    
    void serialize(char* dat) {
        size = data.size() * sizeof(int8_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
    }
    
    static Int8Array deserialize(char* dat) {
        Int8Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        msg.data = std::vector<int8_t>(msg.size/sizeof(uint32_t));
        memcpy(&msg.data[0], dat+sizeof(uint32_t), msg.size);
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(int8_t);
    }
    
    static const uint16_t type = INT8ARRAY_MSG;
};

struct Int16Array: Message {
    uint32_t size;
    std::vector<int16_t> data;
    
    void serialize(char* dat) {
        size = data.size() * sizeof(int16_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
    }
    
    static Int16Array deserialize(char* dat) {
        Int16Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        msg.data = std::vector<int16_t>(msg.size/sizeof(uint32_t));
        memcpy(&msg.data[0], dat+sizeof(uint32_t), msg.size);
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(int16_t);
    }
    
    static const uint16_t type = INT16ARRAY_MSG;
};

struct Int32Array: Message {
    uint32_t size;
    std::vector<int32_t> data;
    
    void serialize(char* dat) {
        size = data.size() * sizeof(int32_t);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
    }
    
    static Int32Array deserialize(char* dat) {
        Int32Array msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        msg.data = std::vector<int32_t>(msg.size/sizeof(uint32_t));
        memcpy(&msg.data[0], dat+sizeof(uint32_t), msg.size);
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(int32_t);
    }
    
    static const uint16_t type = INT32ARRAY_MSG;
};
    // 0x00 0x00 0x00 0x43 0x0a 0x0ffffffd7 0x23 0x3c 0x00 0x00 0x00 0x00 0x0ffffffbe

struct FloatArray: Message {
    uint32_t size;
    std::vector<float> data;
    
    void serialize(char* dat) {
        size = data.size() * sizeof(float);
        memcpy(dat, &size, sizeof(uint32_t));
        memcpy(dat + sizeof(uint32_t), &data[0], size);
    }
    
    static FloatArray deserialize(char* dat) {
        FloatArray msg;
        memcpy(&msg.size, dat, sizeof(uint32_t));
        msg.data = std::vector<float>(msg.size/sizeof(uint32_t));
        memcpy(&msg.data[0], dat+sizeof(uint32_t), msg.size);
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(float);
    }
    
    static const uint16_t type = FLOATARRAY_MSG;
};

}

namespace internal_msg {

enum status_t {
    ATTACHED = 0,
    DETACHED = 1,
    CONNECTED = 2,
    DISCONNECTED = 3
};

struct UpdateMsg: Message {
    
    std::string clientID;
    enum status_t status;
    
    void serialize(char* dat) {
        memcpy(dat, clientID.c_str(), clientID.size() + 1);
        memcpy(dat + sizeof(uint8_t), &status, sizeof(uint8_t));
    }
    
    static UpdateMsg deserialize(char* dat) {
        UpdateMsg msg;
        msg.clientID = std::string(dat);
        std::size_t string_length = std::strlen(dat);
        msg.status = (enum status_t)dat[string_length + 1];
        return msg;
    }
    
    uint32_t getSize() {
        return clientID.length() + 1 + sizeof(uint8_t);
    }
    
    static const uint16_t type = UPDATE_MSG;
};

}

namespace sensor_msg {

struct Image: Message {
    uint16_t width;
    uint16_t height;
    char pixelFormat[4];
    base_msg::UInt8Array pixelArray;
    
    void serialize(char* dat) {
        memcpy(dat, &width, 8);
        pixelArray.serialize(dat + 8);
    }
    
    static Image deserialize(char* dat) {
        Image msg;
        memcpy(&msg, dat, 8);
        msg.pixelArray = base_msg::UInt8Array::deserialize(dat + 8);
        return msg;
    }
    
    uint32_t getSize() {
        return 2 * sizeof(uint16_t) + sizeof(char) * 4 + pixelArray.getSize();
    }
    
    static const uint16_t type = IMAGE_MSG;
};

struct IMU: Message {
    float orientationX;
    float orientationY;
    float orientationZ;
    float angularVelocityX;
    float angularVelocityY;
    float angularVelocityZ;
    float linearAccelerationX;
    float linearAccelerationY;
    float linearAccelerationZ;
    
    void serialize(char* dat) {
        memcpy(dat, &orientationX, sizeof(IMU));
    }
    
    static IMU deserialize(char* dat) {
        IMU msg;
        memcpy(&msg, dat, sizeof(IMU));
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(IMU);
    }
    
    static const uint16_t type = IMU_MSG;
};

}

namespace control_msg {
    
struct Drive: Message {
    float throttle;
    float brake;
    float steer;
    uint8_t reverse;

    
    void serialize(char* dat) {
        memcpy(dat, &throttle, 13);
    }
    
    static Drive deserialize(char* dat) {
        Drive msg;
        memcpy(&msg.throttle, dat, 13);
        return msg;
    }
    
    uint32_t getSize() {
        return 3 * sizeof(float) + sizeof(uint8_t);
    }
    
    static const uint16_t type = DRIVE_MSG;
};

}

namespace nav_msg {

struct Odometry: Message {
    float positionX;
    float positionY;
    float positionZ;
    float roll;
    float pitch;
    float yaw;
    
    void serialize(char* dat) {
        memcpy(dat, &positionX, sizeof(Odometry));
    }
    
    static Odometry deserialize(char* dat) {
        Odometry msg;
        memcpy(&msg.positionX, dat, sizeof(Odometry));
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(Odometry);
    }
    
    static const uint16_t type = ODOMETRY_MSG;
};

}

#endif

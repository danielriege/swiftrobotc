#ifndef msgs
#define msgs

#include <stdint.h>

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
// state_msg
#define VESCSTATUS_MSG      0x0501

struct Message {
    uint32_t serialize(char* data) {}
    static Message deserialize(char* data) {return Message();}
    uint32_t getSize();
    static const uint16_t type = 0;
};

namespace base_msg{

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
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(uint8_t);
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
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(uint16_t);
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
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(uint32_t);
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
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(int8_t);
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
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(int16_t);
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
    
    uint32_t getSize() {
        return sizeof(uint32_t) + data.size() * sizeof(int32_t);
    }
    
    static const uint16_t type = INT32ARRAY_MSG;
};
    // 0x00 0x00 0x00 0x43 0x0a 0x0ffffffd7 0x23 0x3c 0x00 0x00 0x00 0x00 0x0ffffffbe

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
    
    uint32_t getSize() {
        return sizeof(uint8_t) + sizeof(uint8_t);
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
    
    uint32_t serialize(char* dat) {
        memcpy(dat, &width, sizeof(uint16_t));
        memcpy(dat + sizeof(uint16_t), &height, sizeof(uint16_t));
        memcpy(dat + sizeof(uint16_t) + sizeof(uint16_t), &pixelFormat, sizeof(char) * 4);
        pixelArray.serialize(dat + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(char) * 4);
        return sizeof(uint16_t) + sizeof(uint16_t) + sizeof(char) * 4 + pixelArray.getSize();
    }
    
    static Image deserialize(char* dat) {
        Image msg;
        memcpy(&msg.width, dat, sizeof(uint16_t));
        memcpy(&msg.height, dat + sizeof(uint16_t), sizeof(uint16_t));
        memcpy(&msg.pixelFormat, dat + sizeof(uint16_t) + sizeof(uint16_t), sizeof(char) * 4);
        msg.pixelArray = base_msg::UInt8Array::deserialize(dat + sizeof(uint16_t) + sizeof(uint16_t) + sizeof(char) * 4);
        return msg;
    }
    
    uint32_t getSize() {
        return sizeof(uint16_t) + sizeof(uint16_t) + sizeof(char) * 4 + pixelArray.getSize();
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
    
    uint32_t serialize(char* dat) {
        memcpy(dat, &orientationX, sizeof(float));
        memcpy(dat + sizeof(float), &orientationY, sizeof(float));
        memcpy(dat + 2 * sizeof(float), &orientationZ, sizeof(float));
        memcpy(dat + 3 * sizeof(float), &angularVelocityX, sizeof(float));
        memcpy(dat + 4 * sizeof(float), &angularVelocityY, sizeof(float));
        memcpy(dat + 5 * sizeof(float), &angularVelocityZ, sizeof(float));
        memcpy(dat + 6 * sizeof(float), &linearAccelerationX, sizeof(float));
        memcpy(dat + 7 * sizeof(float), &linearAccelerationY, sizeof(float));
        memcpy(dat + 8 * sizeof(float), &linearAccelerationZ, sizeof(float));
        return 9 * sizeof(float);
    }
    
    static IMU deserialize(char* dat) {
        IMU msg;
        memcpy(&msg.orientationX, dat, sizeof(float));
        memcpy(&msg.orientationY, dat + sizeof(float), sizeof(float));
        memcpy(&msg.orientationZ, dat + 2 * sizeof(float), sizeof(float));
        memcpy(&msg.angularVelocityX, dat + 3 * sizeof(float), sizeof(float));
        memcpy(&msg.angularVelocityY, dat + 4 * sizeof(float), sizeof(float));
        memcpy(&msg.angularVelocityZ, dat + 5 * sizeof(float), sizeof(float));
        memcpy(&msg.linearAccelerationX, dat + 6 * sizeof(float), sizeof(float));
        memcpy(&msg.linearAccelerationY, dat + 7 * sizeof(float), sizeof(float));
        memcpy(&msg.linearAccelerationZ, dat + 8 * sizeof(float), sizeof(float));
        return msg;
    }
    
    uint32_t getSize() {
        return 9 * sizeof(float);
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

    
    uint32_t serialize(char* dat) {
        memcpy(dat, &throttle, sizeof(float));
        memcpy(dat + sizeof(float), &brake, sizeof(float));
        memcpy(dat + 2 * sizeof(float), &steer, sizeof(float));
        memcpy(dat + 3 * sizeof(float), &reverse, sizeof(uint8_t));
        return 3 * sizeof(float) + sizeof(uint8_t);
    }
    
    static Drive deserialize(char* dat) {
        Drive msg;
        memcpy(&msg.throttle, dat, sizeof(float));
        memcpy(&msg.brake, dat + sizeof(float), sizeof(float));
        memcpy(&msg.steer, dat + 2 * sizeof(float), sizeof(float));
        memcpy(&msg.reverse, dat + 3 * sizeof(float), sizeof(uint8_t));
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
    
    uint32_t serialize(char* dat) {
        memcpy(dat, &positionX, sizeof(float));
        memcpy(dat + sizeof(float), &positionY, sizeof(float));
        memcpy(dat + 2 * sizeof(float), &positionZ, sizeof(float));
        memcpy(dat + 3 * sizeof(float), &roll, sizeof(float));
        memcpy(dat + 4 * sizeof(float), &pitch, sizeof(float));
        memcpy(dat + 5 * sizeof(float), &yaw, sizeof(float));
        return 6 * sizeof(float);
    }
    
    static Odometry deserialize(char* dat) {
        Odometry msg;
        memcpy(&msg.positionX, dat, sizeof(float));
        memcpy(&msg.positionY, dat + sizeof(float), sizeof(float));
        memcpy(&msg.positionZ, dat + 2 * sizeof(float), sizeof(float));
        memcpy(&msg.roll, dat + 3 * sizeof(float), sizeof(float));
        memcpy(&msg.pitch, dat + 4 * sizeof(float), sizeof(float));
        memcpy(&msg.yaw, dat + 5 * sizeof(float), sizeof(float));
        return msg;
    }
    
    uint32_t getSize() {
        return 6 * sizeof(float);
    }
    
    static const uint16_t type = ODOMETRY_MSG;
};

}

namespace state_msg {

struct VescStatus: Message {
    float mosfet_temp;
    float motor_temp;
    int32_t rpm;
    float battery_voltage;
    int32_t tachometer;
    int32_t tachometer_abs;
    
    uint32_t serialize(char* dat) {
        memcpy(dat, &mosfet_temp, sizeof(float));
        memcpy(dat + sizeof(float), &motor_temp, sizeof(float));
        memcpy(dat + 2 * sizeof(float), &rpm, sizeof(int32_t));
        memcpy(dat + 2 * sizeof(float) + sizeof(int32_t), &battery_voltage, sizeof(float));
        memcpy(dat + 3 * sizeof(float) + sizeof(int32_t), &tachometer, sizeof(int32_t));
        memcpy(dat + 3 * sizeof(float) + 2 * sizeof(int32_t), &tachometer_abs, sizeof(int32_t));
        return 3 * sizeof(float) + 3 * sizeof(int32_t);
    }
    
    static VescStatus deserialize(char* dat) {
        VescStatus msg;
        memcpy(&msg.mosfet_temp, dat, sizeof(float));
        memcpy(&msg.motor_temp, dat + sizeof(float), sizeof(float));
        memcpy(&msg.rpm, dat + 2 * sizeof(float), sizeof(int32_t));
        memcpy(&msg.battery_voltage, dat + 2 * sizeof(float) + sizeof(int32_t), sizeof(float));
        memcpy(&msg.tachometer, dat + 3 * sizeof(float) + sizeof(int32_t), sizeof(int32_t));
        memcpy(&msg.tachometer_abs, dat + 3 * sizeof(float) + 2 * sizeof(int32_t), sizeof(int32_t));
        return msg;
    }
    
    uint32_t getSize() {
        return 3 * sizeof(float) + 3 * sizeof(int32_t);
    }
    
    static const uint16_t type = VESCSTATUS_MSG;
};

}

#endif

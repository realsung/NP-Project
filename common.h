typedef unsigned char uint8;

const uint8 PACKET_HEADER=0x55;

enum PacketType {    // 열거형 정의
    ASSIGN_WORK = 1,
    WORK_READY = 2,
    WORK_PROGRESS = 3,
    WORK_COMPLETE = 4,
    ACK = 5
};
typedef struct AssignWorkPacket {
    
} AssignWorkPacket;
//
// Created by paul on 02.10.17.
//
#include "rcLib.hpp"

uint8_t rcLib::Package::globalPackageUid = 0;
uint8_t rcLib::Package::transmitterId = 0;
uint8_t rcLib::Package::errorCount = 0;

rcLib::Package::Package() = default;

rcLib::Package::Package(uint16_t resolution, uint8_t channelCount) {
    this->resolution = resolution;
    this->channelCount = channelCount;
    this->mesh = false;
    this->discoverState = 0;
}

uint8_t rcLib::Package::encode() {
    buffer[0] = RC_LIB_START;
    buffer[1] = ++globalPackageUid;
    buffer[2] = transmitterId;
    mesh = mesh?1:0;
    buffer[3] = static_cast<uint8_t>(resolutionStepsToKey(resolution) |
                                     channelCountToKey(channelCount) << 3 |
                                     (errorCount>0?1:0) << 6 |
                                     mesh << 7);
    if(mesh){
        buffer[4] = routingLength | (discoverState == 1) << 5  | (discoverState == 2) << 6;
    }

    uint8_t resBits = resolutionStepsToBitCount(resolution);

    uint16_t dataSize = resBits * channelCount;
    if(dataSize % 8 == 0){
        dataSize /= 8;
    } else {
        dataSize = dataSize/8 + 1;
    }

    for(int c=0; c<dataSize; c++){
        buffer[4+c+mesh] = 0;
        for(int b=0; b<8 && (c*8+b)<(resBits*channelCount); b++){
            uint8_t bit = static_cast<uint8_t>(channelData[(c * 8 + b) / resBits] & (0b1 << ((c * 8 + b) % resBits)) ? 1:0);
            buffer[4+c+mesh] |= bit << b;
        }
    }

    bufCount = 4+dataSize+2+mesh;
    buffer[4+dataSize+mesh] = calculateChecksum(buffer, bufCount);
    buffer[4+dataSize+mesh+1] = RC_LIB_END;

    return bufCount;
}

uint8_t rcLib::Package::decode(uint8_t data) {
    static uint8_t receiveStateMachineState = 0;
    static uint8_t dataByteCount = 0;

    switch(receiveStateMachineState){
        case 0: // Initial state
            bufCount = 0;
            if(data == RC_LIB_START) {
                receiveStateMachineState = 1;
            }
            break;
        case 1: // Start word received
            this->uid = data;
            globalPackageUid = this->uid;

            receiveStateMachineState = 2;
            break;
        case 2: // Transmitter Id
            this->tid = data;
            receiveStateMachineState = 3;
            break;
        case 3: // Configuration
            this->resolution = keyToResolutionSteps(data&0b111);
            this->channelCount = keyToChannelCount((data&0b111000) >> 3);

            for(int c=0; c<channelCount; c++){
                channelData[c] = 0;
            }

            // Following
            if(data & (0b1 << 7)){
                receiveStateMachineState = 4;
            } else {
                receiveStateMachineState = 5;
            }
            dataByteCount = 0;
            break;
        case 4: // Mesh
            this->routingLength = data & 0b1111;
            this->discoverState = (data & (0b11 << 5)) >> 5;
            this->mesh = (this->routingLength > 0);
            if(this->discoverState == 1) {
                receiveStateMachineState = 6;
            } else {
                receiveStateMachineState = 5;
            }
            break;
        case 5: // Data
            {
                uint8_t resBits = resolutionStepsToBitCount(resolution);

                uint16_t dataSize = resBits * channelCount;
                if (dataSize % 8 == 0) {
                    dataSize /= 8;
                } else {
                    dataSize = dataSize / 8 + 1;
                }

                for (int c = 0; c < 8; c++) {
                    uint8_t bit = (dataByteCount * 8 + c) % resBits;
                    this->channelData[(dataByteCount * 8 + c) / resBits] |= ((data & (0b1 << c))?1:0) << bit;
                }
                if (++dataByteCount >= dataSize) {
                    receiveStateMachineState = 6;
                }
            }
            break;
        case 6: // Checksum
            this->checksum = data;
            receiveStateMachineState = 7;

            if(!isChecksumCorrect()){
                errorCount += 4;
            }
            break;
        case 7: // End byte
            if(data == RC_LIB_END) {
                receiveStateMachineState = 0;
                bufCount++;
                errorCount--;
                return static_cast<uint8_t>(true);
            } else {
                errorCount += 4;
                receiveStateMachineState = 0;
                return static_cast<uint8_t>(false);
            }
        default:
            receiveStateMachineState = 0;
            break;
    }
    buffer[bufCount++] = data;

    return static_cast<uint8_t>(false);
}

void rcLib::Package::setChannel(uint8_t channel, uint16_t data) {
    if(data < 0){
        data = 0;
    } else if(data >= this->resolution){
        data = static_cast<uint16_t>(this->resolution - 1);
    }

    if(channel >= 0 && channel < this->channelCount) {
        this->channelData[channel] = data;
    }
}

void rcLib::Package::setMeshProperties(uint8_t enabled, uint8_t routingLength) {
    this->mesh = enabled;
    this->routingLength = routingLength;
}

uint8_t rcLib::Package::getDeviceId() {
    return this->tid;
}

uint16_t rcLib::Package::getResolution() {
    return this->resolution;
}

uint16_t rcLib::Package::getChannel(uint8_t channel) {
    if(channel >= 0 && channel <= this->channelCount){
        return this->channelData[channel];
    } else {
        return static_cast<uint16_t>(-1);
    }
}

uint16_t rcLib::Package::getChannelCount() {
    return channelCount;
}

uint8_t rcLib::Package::isChecksumCorrect() {
    return static_cast<uint8_t>(this->checksum == this->calculateChecksum());
}

uint8_t rcLib::Package::isMesh() {
    return this->mesh;
}

uint8_t rcLib::Package::needsForwarding() {
    return static_cast<uint8_t>(this->mesh && this->routingLength > 0);
}

void rcLib::Package::countNode() {
    this->routingLength--;
}

uint8_t rcLib::Package::isDiscoverMessage() {
    return this->discoverState == 1;
}


uint8_t rcLib::Package::isDiscoverResponse() {
    return this->discoverState == 2;
}

void rcLib::Package::setDiscoverMessage() {
    this->discoverState = 1;
    this->channelCount = 0;
    this->mesh = 1;
}

void rcLib::Package::makeDiscoverResponse(Package responses[], uint8_t len) {
    this->resolution = 256;
    this->channelCount = 1;
    this->mesh = 1;
    this->discoverState = 2;

    uint16_t tidCount = 0;
    for(uint8_t r=0; r<len; r++) {
        for(uint16_t c=0; c<responses[r].getChannelCount(); c++) {
            if(responses[r].getChannel(c) != 0) {
                if(tidCount+1 > this->channelCount) {
                    this->channelCount *= 2;
                }

                this->setChannel(tidCount++, responses[r].getChannel(c));
            }
        }
    }

    if(tidCount+1 > this->channelCount) {
        this->channelCount *= 2;
    }
    this->setChannel(tidCount++, this->transmitterId);
    for(uint16_t c=tidCount; c<this->channelCount; c++) {
        this->setChannel(c, 0);
    }
}

uint8_t rcLib::Package::calculateChecksum(uint8_t* data, uint8_t size) {
    uint8_t checksum = 0;

    for(uint16_t c=0; c<size-3; c++){
        checksum ^= data[c+1];
    }

    return checksum;
}

uint8_t rcLib::Package::calculateChecksum(void) {
    return calculateChecksum(this->buffer, bufCount);
}

uint8_t rcLib::Package::resolutionStepsToKey(uint16_t steps) {
    switch(steps){
        case 32:
            return 0b000;
        case 64:
            return 0b001;
        case 128:
            return 0b010;
        case 256:
            return 0b011;
        case 512:
            return 0b100;
        case 1024:
            return 0b101;
        case 2048:
            return 0b110;
        case 4096:
            return 0b111;
        default:
            return 0b111;
    }
}

uint8_t rcLib::Package::channelCountToKey(uint16_t channelCount) {
    switch(channelCount){
        case 0:
        case 1:
            return 0b000;
        case 2:
            return 0b001;
        case 4:
            return 0b010;
        case 8:
            return 0b011;
        case 16:
            return 0b100;
        case 32:
            return 0b101;
        case 64:
            return 0b110;
        case 256:
            return 0b111;
        default:
            return 0b111;
    }
}

uint8_t rcLib::Package::resolutionStepsToBitCount(uint16_t steps) {
    switch(steps){
        case 32:
            return 5;
        case 64:
            return 6;
        case 128:
            return 7;
        case 256:
            return 8;
        case 512:
            return 9;
        case 1024:
            return 10;
        case 2048:
            return 11;
        case 4096:
            return 12;
        default:
            return 12;
    }
}

uint16_t rcLib::Package::keyToResolutionSteps(uint8_t key) {
    switch(key){
        case 0b000:
            return 32;
        case 0b001:
            return 64;
        case 0b010:
            return 128;
        case 0b011:
            return 256;
        case 0b100:
            return 512;
        case 0b101:
            return 1024;
        case 0b110:
            return 2048;
        case 0b111:
            return 4096;
        default:
            return 4096;
    }
}

uint16_t rcLib::Package::keyToChannelCount(uint8_t key) {
    switch(key){
        case 0b000:
            return 1;
        case 0b001:
            return 2;
        case 0b010:
            return 4;
        case 0b011:
            return 8;
        case 0b100:
            return 16;
        case 0b101:
            return 32;
        case 0b110:
            return 64;
        case 0b111:
            return 256;
        default:
            return 256;
    }
}

uint8_t *rcLib::Package::getEncodedData() {
    return buffer;
}

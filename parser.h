#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdint>

struct Vector3 {
    float x, y, z;
};

struct NounData {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    std::string modelName;
    std::string physicsProps;
    std::vector<std::string> unknownFields;
};

class NounParser {
private:

    static void printHex(const std::vector<uint8_t>& data, size_t start, size_t length) {
        for (size_t i = start; i < start + length && i < data.size(); ++i) {
            std::cout << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(data[i]) << " ";
        }
        std::cout << std::dec << std::endl;
    }

    static float readFloat(const std::vector<uint8_t>& data, size_t& offset) {
        float value;
        memcpy(&value, &data[offset], sizeof(float));
        offset += sizeof(float);
        return value;
    }

    static Vector3 readVector3(const std::vector<uint8_t>& data, size_t& offset) {
        Vector3 vec;
        vec.x = readFloat(data, offset);
        vec.y = readFloat(data, offset);
        vec.z = readFloat(data, offset);
        return vec;
    }

    static std::string readString(const std::vector<uint8_t>& data, size_t& offset) {
        std::string str;
        size_t start = offset;
        while (offset < data.size() && data[offset] != 0) {
            offset++;
        }
        str = std::string(reinterpret_cast<const char*>(&data[start]), offset - start);
        offset++;
        return str;
    }
    
    static std::string readModelName(const std::vector<uint8_t>& data, size_t& offset) {
            while (offset < data.size() - 7) {
                if (data[offset] == 'E' && data[offset+1] == 'f' && data[offset+2] == 'f' &&
                    data[offset+3] == 'e' && data[offset+4] == 'c' && data[offset+5] == 't' &&
                    data[offset+6] == 's' && data[offset+7] == '!') {
                    break;
                }
                offset++;
            }

            std::string modelName;
            while (offset < data.size() && data[offset] != 0) {
                modelName += static_cast<char>(data[offset]);
                offset++;
            }
            offset++;
            return modelName;
        }
public:
    static NounData ParseNounFile(const std::vector<uint8_t>& data) {
        NounData noun;
        size_t offset = 0;

        offset = 20;

        noun.position = readVector3(data, offset);
        noun.rotation = readVector3(data, offset);
        noun.scale = readVector3(data, offset);

        std::cout << "Position bytes: ";
        printHex(data, offset, 12);
        noun.position.x = readFloat(data, offset);
        noun.position.y = readFloat(data, offset);
        noun.position.z = readFloat(data, offset);

        std::cout << "Rotation bytes: ";
        printHex(data, offset, 12);
        noun.rotation.x = readFloat(data, offset);
        noun.rotation.y = readFloat(data, offset);
        noun.rotation.z = readFloat(data, offset);

        std::cout << "Scale bytes: ";
        printHex(data, offset, 12);
        noun.scale.x = readFloat(data, offset);
        noun.scale.y = readFloat(data, offset);
        noun.scale.z = readFloat(data, offset);

        while (offset < data.size() && data[offset] == 0) {
            offset++;
        }

        noun.modelName = readModelName(data, offset);

        noun.physicsProps = readString(data, offset);

        while (offset < data.size()) {
            std::string str = readString(data, offset);
            if (str == "UNKNOWN") {
                noun.unknownFields.push_back(str);
            }
        }

        return noun;
    }
};

std::string NounToXML(const NounData& noun) {
    std::stringstream xml;
    xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    xml << "<noun>\n";

    xml << std::fixed << std::setprecision(7);

    xml << "  <position x=\"" << noun.position.x 
        << "\" y=\"" << noun.position.y 
        << "\" z=\"" << noun.position.z << "\"/>\n";

    xml << "  <rotation x=\"" << noun.rotation.x 
        << "\" y=\"" << noun.rotation.y 
        << "\" z=\"" << noun.rotation.z << "\"/>\n";

    xml << "  <scale x=\"" << noun.scale.x 
        << "\" y=\"" << noun.scale.y 
        << "\" z=\"" << noun.scale.z << "\"/>\n";

    if (!noun.modelName.empty()) {
        xml << "  <model>" << noun.modelName << "</model>\n";
    }

    if (!noun.physicsProps.empty()) {
        xml << "  <physics>" << noun.physicsProps << "</physics>\n";
    }

    if (!noun.unknownFields.empty()) {
        xml << "  <unknownData>\n";
        for (const auto& field : noun.unknownFields) {
            xml << "    <field>" << field << "</field>\n";
        }
        xml << "  </unknownData>\n";
    }

    xml << "</noun>";
    return xml.str();
}
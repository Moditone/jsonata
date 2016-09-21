#include <cassert>
#include <string>

#include "writer.hpp"

using namespace std;

namespace json
{    
    Writer::Writer(const Value& value) :
        value(value)
    {
        
    }

    void writeString(ostream& stream, const string& string)
    {
        stream << '\"';
        for (auto& c : string)
        {
            switch (c)
            {
                case '\"': stream << "\\\""; break;
                case '\\': stream << "\\\\"; break;
                default: stream << c; break;
            }
        }
        
        stream << '\"';
    }
    
    ostream& operator<<(ostream& stream, const Writer& writer)
    {
        if (writer.value.isNull())
            return stream << "null";
        else if (writer.value.isBool())
            return stream << (writer.value.asBool() ? "true" : "false");
        else if (writer.value.isNumber())
            return stream << writer.value.asNumber();
        else if (writer.value.isString())
        {
            writeString(stream, writer.value.asString());
            return stream;
        } else if (writer.value.isArray()) {
            stream << '[';
            
            const auto size = writer.value.size();
            for (auto i = 0; i < size; ++i)
            {
                stream << writer.value[i];
                if (i != size - 1)
                    stream << ',';
            }
            
            return stream << ']';
        } else if (writer.value.isObject()) {
            stream << '{';
            
            const auto size = writer.value.size();
            const auto keys = writer.value.keys();
            for (auto i = 0; i < size; ++i)
            {
                writeString(stream, keys[i]);
                stream << ":" << writer.value[keys[i]];;
                if (i != size - 1)
                    stream << ',';
            }
            
            return stream << '}';
        }
        
        assert(false);
    }
}
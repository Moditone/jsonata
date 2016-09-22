#include <cassert>
#include <string>

#include "writer.hpp"

using namespace std;

namespace json
{
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
    
    Writer::Writer(const Value& value) :
        value(value)
    {
        
    }
    
    void Writer::writeToStream(ostream& stream) const
    {
        if (value.isNull())
            stream << "null";
        else if (value.isBool())
            stream << (value.asBool() ? "true" : "false");
        else if (value.isNumber())
            stream << value.asNumber();
        else if (value.isString())
        {
            writeString(stream, value.asString());
        } else if (value.isArray()) {
            stream << '[';
            
            const auto size = value.size();
            for (auto i = 0; i < size; ++i)
            {
                stream << value[i];
                if (i != size - 1)
                    stream << ',';
            }
            
            stream << ']';
        } else if (value.isObject()) {
            stream << '{';
            
            const auto size = value.size();
            const auto keys = value.keys();
            for (auto i = 0; i < size; ++i)
            {
                writeString(stream, keys[i]);
                stream << ":" << value[keys[i]];;
                if (i != size - 1)
                    stream << ',';
            }
            
            stream << '}';
        }
    }
    
    ostream& operator<<(ostream& stream, const Writer& writer)
    {
        writer.writeToStream(stream);
        return stream;
    }
}

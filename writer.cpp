#include <cassert>
#include <string>

#include "writer.hpp"

using namespace std;

namespace json
{
    Writer::Writer(const Value& value, const Format& format) :
        value(value),
        format(format)
    {
        
    }
    
    void Writer::Format::write(ostream& stream, const Value& value) const
    {
        if (value.isNull())
            stream << "null";
        else if (value.isBool())
            stream << (value.asBool() ? "true" : "false");
        else if (value.isNumber())
            stream << value.asNumber();
        else if (value.isString())
        {
            write(stream, value.asString());
        } else if (value.isArray()) {
            stream << '[';
            
            const auto size = value.size();
            for (auto i = 0; i < size; ++i)
            {
                write(stream, value[i]);
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
                write(stream, keys[i]);
                stream << ":";
                write(stream, value[keys[i]]);
                
                if (i != size - 1)
                    stream << ',';
            }
            
            stream << '}';
        }
    }
    
    void Writer::Format::write(ostream& stream, const string& string) const
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
    
    void Writer::PrettyFormat::write(ostream& stream, const Value& value) const
    {
        return Format::write(stream, value);
    }
    
    ostream& operator<<(ostream& stream, const Writer& writer)
    {
        writer.format.write(stream, writer.value);
        return stream;
    }
}

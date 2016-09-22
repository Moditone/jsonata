#include <cassert>
#include <sstream>
#include <string>

#include "writer.hpp"

using namespace std;

namespace json
{
    string Writer::writeToString(const Value& value) const
    {
        stringstream stream;
        writeToStream(stream, value);
        return stream.str();
    }
    
    void LeanWriter::writeToStream(ostream& stream, const Value& value) const
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
                writeToStream(stream, value[i]);
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
                writeToStream(stream, value[keys[i]]);
                
                if (i != size - 1)
                    stream << ',';
            }
            
            stream << '}';
        }
    }
    
    void LeanWriter::write(ostream& stream, const string& string) const
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
    
    void PrettyWriter::writeToStream(ostream& stream, const Value& value) const
    {
        return LeanWriter::writeToStream(stream, value);
    }
    
    Streamer::Streamer(const Value& value, const Writer& writer) :
        value(value),
        writer(writer)
    {
        
    }
    
    ostream& operator<<(ostream& stream, const Streamer& streamer)
    {
        streamer.writer.writeToStream(stream, streamer.value);
        return stream;
    }
}

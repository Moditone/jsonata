#include <cassert>
#include <sstream>
#include <string>

#include "writer.hpp"

using namespace std;

namespace json
{
    
// --- Writer --- //
    
    string Writer::writeToString(const Value& value) const
    {
        ostringstream stream;
        writeToStream(stream, value);
        return stream.str();
    }
    
// --- LeanWriter --- //
    
    void LeanWriter::writeToStream(ostream& stream, const Value& value) const
    {
        if (value.isNull())
            stream << "null";
        else if (value.isBool())
            stream << (value.asBool() ? "true" : "false");
        else if (value.isSignedInteger())
            stream << value.asSignedInteger();
        else if (value.isUnsignedInteger())
            stream << value.asUnsignedInteger();
        else if (value.isReal())
            stream << fixed << value.asReal();
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
    
// --- PrettyWriter --- //
    
    void PrettyWriter::writeToStream(ostream& stream, const Value& value) const
    {
        writeToStreamWithIndentation(stream, value, 0);
    }
    
    void PrettyWriter::writeToStreamWithIndentation(ostream& stream, const Value& value, unsigned int indentation) const
    {
        if (value.isArray())
        {
            stream << "[\n";
            ++indentation;
            
            const auto size = value.size();
            for (auto i = 0; i < size; ++i)
            {
                writeIndentation(stream, indentation);
                writeToStreamWithIndentation(stream, value[i], indentation);
                if (i != size - 1)
                    stream << ',';
                
                stream << '\n';
            }
            
            --indentation;
            writeIndentation(stream, indentation);
            
            stream << ']';
        } else if (value.isObject()) {
            stream << "{\n";
            ++indentation;
            
            const auto size = value.size();
            const auto keys = value.keys();
            for (auto i = 0; i < size; ++i)
            {
                writeIndentation(stream, indentation);
                write(stream, keys[i]);
                stream << ": ";
                writeToStreamWithIndentation(stream, value[keys[i]], indentation);
                
                if (i != size - 1)
                    stream << ',';
                
                stream << '\n';
            }
            
            --indentation;
            writeIndentation(stream, indentation);
            
            stream << '}';
        } else {
            return LeanWriter::writeToStream(stream, value);
        }
    }
    
    void PrettyWriter::writeIndentation(ostream& stream, unsigned int indentation) const
    {
        for (auto i = 0; i < indentation; ++i)
            stream << '\t';
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
    
    ostream& operator<<(ostream& stream, const Value& value)
    {
        return stream << Streamer(value, PrettyWriter{});
    }
}

//
//  writer.hpp
//  Jsonata
//
//  Copyright © 2015-2016 Dsperados (info@dsperados.com). All rights reserved.
//  Licensed under the BSD 3-clause license.
//

#include <cassert>
#include <iomanip>
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
        {
            stringstream stream2;
            stream2 << fixed;
            stream2.precision(static_cast<long>(realPrecision));
            stream2 << value.asReal();
            auto str = stream2.str();
            
            while (str.back() == '0')
                str.pop_back();
            
            if (str.back() == '.')
                str.pop_back();
            
            stream << str;
        } else if (value.isString()) {
            stream << std::quoted(value.asString());
        } else if (value.isArray()) {
            stream << '[';
            
            const auto size = value.size();
            for (std::size_t i = 0; i < size; ++i)
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
            for (std::size_t i = 0; i < size; ++i)
            {
                stream << std::quoted(keys[i]) << ":";
                writeToStream(stream, value[keys[i]]);
                
                if (i != size - 1)
                    stream << ',';
            }
            
            stream << '}';
        }
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
            if (value.empty())
            {
                stream << "[]";
                return;
            }
            
            stream << "[\n";
            ++indentation;
            
            const auto size = value.size();
            for (std::size_t i = 0; i < size; ++i)
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
            if (value.empty())
            {
                stream << "{}";
                return;
            }
            
            stream << "{\n";
            ++indentation;
            
            const auto size = value.size();
            const auto keys = value.keys();
            for (std::size_t i = 0; i < size; ++i)
            {
                writeIndentation(stream, indentation);
                stream << std::quoted(keys[i]) << ": ";
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

#include <ofem/calfem_reader.h>

#include <filesystem>
#include <iostream>
#include <fstream>
#include <string>

#include "base64.h"

using namespace ofem;
using namespace std;

namespace fs = std::filesystem;


CalfemReader::CalfemReader(const std::string fname) : m_filename{fname}, m_isValid{false}
{
    m_isValid = this->parse();
}

bool ofem::CalfemReader::parse()
{
    if (m_filename != "")
    {
        std::ifstream in(m_filename);
        this->readFromStream(in);
        in.close();
        return m_isValid;
    }
    else
        return false;
}

void CalfemReader::readFromStream(std::istream &in)
{
    std::string line;
    std::string base64_string("");
    bool begin_parse = false;
    bool found_base64 = false;
    std::string base64_line("");

    m_isValid = false;

    while (std::getline(in, line)) 
    {
        if (line.find("df3_file") != -1)
        {
            auto start_triple = line.find("'''");
            
            if (start_triple != -1)
            {
                auto base64_line = line.substr(start_triple + 3);
                base64_string += base64_line;

                begin_parse = true;
                continue;
            }
        }

        if (begin_parse)
        {
            if (line.find("'''") != -1)
            {
                auto start_triple = line.find("'''");
                auto base64_line = line.substr(0, start_triple);
                base64_string += base64_line;
                begin_parse = false;
                found_base64 = true;
            }
            else
            {
                auto base64_line = line;
                base64_string += base64_line;
            }
        }
    }

    if (found_base64) 
    {
        m_base64String = base64_string;
        m_df3String = base64_decode(m_base64String);

        m_isValid = true;
    }
}

std::string ofem::CalfemReader::base64String()
{
    return m_base64String;
}

std::string ofem::CalfemReader::df3String()
{
    return m_df3String;
}

bool ofem::CalfemReader::isValid()
{
    return m_isValid;
}

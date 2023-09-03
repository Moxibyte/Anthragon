#include "blob.h"

ant::sdf::blob::blob(const std::filesystem::path& file)
{
    std::ifstream file_in(file, std::ios::binary);
    ANT_CHECK(file_in.is_open(), "Can't open file");

    file_in.seekg(0, std::ios::end);
    size_t file_size = file_in.tellg();
    file_in.seekg(0, std::ios::beg);
    
    m_data.resize(file_size);
    file_in.read(m_data.data(), file_size);
}

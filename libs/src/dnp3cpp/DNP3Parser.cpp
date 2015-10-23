#include "dnp3cpp/DNP3Parser.h"

#include <easylogging++.h>

extern "C"
{
  #include <plugin.h>
}

void error(const char *fmt, ...)
{
    char buffer[80];
    va_list args;
    va_start(args, fmt);
    snprintf(buffer, 80, fmt, args);
    va_end(args);

    LOG(ERROR) << buffer;
}

void debug_(const char *fmt, ...)
{
    char buffer[80];
    va_list args;
    va_start(args, fmt);
    snprintf(buffer, 80, fmt, args);
    va_end(args);

    LOG(DEBUG) << buffer;
}

namespace proxy { namespace  dnp3 {

WSlice DNP3Parser::GetWriteSlice()
{
    return WSlice(m_plugin->buf, m_plugin->bufsize);
}

bool DNP3Parser::Parse(size_t num)
{
    return m_plugin->feed(m_plugin, num);
}

void DNP3Parser::QueueOutput(void *env, const uint8_t *buf, size_t n)
{
    auto parser = reinterpret_cast<DNP3Parser*>(env);
    parser->m_callbacks.QueueWrite(RSlice(buf, n));
}

DNP3Parser::DNP3Parser(IParserCallbacks& callbacks) :
        m_callbacks(callbacks),
        m_plugin(dnp3_dissect(&QueueOutput, this))
{
    if(m_plugin == nullptr)
    {
        throw std::runtime_error("DNP3 plugin bind failed");
    }
}

DNP3Parser::~DNP3Parser()
{
    if(m_plugin)
    {
        m_plugin->finish(m_plugin);
    }
}

}}

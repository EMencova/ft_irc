#pragma once


class Channel
{
    public:
        std::map<std::string, Client> getMembers() const;
};
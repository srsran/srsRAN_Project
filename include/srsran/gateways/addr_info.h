#pragma once

#include "netinet/in.h"

inline const char* ipproto_to_string(int protocol)
{
  switch (protocol) {
    case IPPROTO_SCTP:
      return "SCTP";
    case IPPROTO_UDP:
      return "UDP";
    case IPPROTO_IP:
      return "IP";
    case IPPROTO_IPV6:
      return "IPV6";
    case IPPROTO_ICMP:
      return "ICMP";
    case IPPROTO_TCP:
      return "TCP";
    default:
      break;
  }
  return "unknown protocol";
}

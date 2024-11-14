/*
 *
 * Copyright 2021-2024 Software Radio Systems Limited
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the distribution.
 *
 */

#pragma once

#include <netinet/in.h>

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

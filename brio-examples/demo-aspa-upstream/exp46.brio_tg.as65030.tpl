mode = "BGP";
disable_bgpsec = true;

session = (
  {
    asn        = 65030;
    bgp_ident  = "{IP_AS_65030-46}";
    hold_timer = 180;

    local_addr = "{IP_AS_65030-46}";

    peer_asn   = 65020;
    peer_ip    = "{IP_AS_65020-46}";
    peer_port  = 179;

    disconnect = 0;

    # (path prefix B4 specifies BGP4 only update!)
    # <prefix>[,[[B4]? <asn>[p<repetition>]]*[ ]*[I|V|N]?]
    #
    #  Topology as announced by brio_tg using BGP UPDATES:
    #
    #           65020-----65030-----65040
    #            (C)       (D)       (E)
    #       /|\                        \
    #   C2P  |                          \
    #        |                           \
    #                                     \
    #                                    65010 (B)
    #
    #   BRIO: D
    #   IUT:  C
    #
    #   The complete topology is described in the examples README file.
    #
    update = (  
              "10.65.10.0/24, B4 65040 65010"
             );

    printOnSend = {
      update       = true;
    };
    
    printSimple    = true;
  }
);

update = ( 
         );

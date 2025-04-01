mode = "BGP";
disable_bgpsec = true;

session = (
  {
    asn        = 65040;
    bgp_ident  = "{IP_AS_65040-1}";
    hold_timer = 180;

    local_addr = "{IP_AS_65040-1}";

    peer_asn   = 65010;
    peer_ip    = "{IP_AS_65010-1}";
    peer_port  = 179;

    disconnect = 0;

    # (path prefix B4 specifies BGP4 only update!)
    # <prefix>[,[[B4]? <asn>[p<repetition>]]*[ ]*[I|V|N]?]
    #
    #  Topology as announced by brio_tg using BGP UPDATES:
    #                65050     65060
    #                 [F]-------(G) {AS 0 ASPA}
    #       /|\       /  <-p2p->  \ 
    #   C2P  |       /             \ 
    #        |      /               \ 
    #              /                 \
    #           65020               65040
    #            (C)                 [E]
    #       /|\    \                   \
    #   C2P  |      \                   \  
    #        |       \                   \ 
    #                 \                   \ 
    #                65000 (A)           65010 (B)
    #
    #   BRIO: E
    #   IUT:  B
    #
    #   The complete topology is described in the examples README file.
    #
    update = (  
              "10.65.0.0/22, B4 65060 65050 65020 65000"
             );

    printOnSend = {
      update       = true;
    };
    
    printSimple    = true;
  }
);

update = ( 
         );

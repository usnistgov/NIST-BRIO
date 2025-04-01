mode = "BGP";
disable_bgpsec = true;

session = (
  {
    asn        = 65040;
    bgp_ident  = "{IP_AS_65040-79}";
    hold_timer = 180;

    local_addr = "{IP_AS_65040-79}";

    peer_asn   = 65020;
    peer_ip    = "{IP_AS_65030-79}";
    peer_port  = 179;

    disconnect = 0;

    # (path prefix B4 specifies BGP4 only update!)
    # <prefix>[,[[B4]? <asn>[p<repetition>]]*[ ]*[I|V|N]?]
    #
    #  Topology as announced by brio_tg using BGP UPDATES:
    #
    #               <-p2p->
    #            (D)-------(E)
    #       /|\              \
    #   C2P  |                \
    #        |                 \
    #                           \
    #                           (B)
    #
    #   BRIO: E
    #   IUT:  D
    #
    #   The complete topology is described in the examples README file.
    #
    update = (  
              "10.65.10.0/22, B4 65040 65010"
             );

    printOnSend = {
      update       = true;
    };
    
    printSimple    = true;
  }
);

update = ( 
         );

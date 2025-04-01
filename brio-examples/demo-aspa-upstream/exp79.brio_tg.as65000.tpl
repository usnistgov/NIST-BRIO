mode = "BGP";
disable_bgpsec = true;

session = (
  {
    asn        = 65000;
    bgp_ident  = "{IP_AS_65000-46}";
    hold_timer = 180;

    local_addr = "{IP_AS_65000-46}";

    peer_asn   = 65020;
    peer_ip    = "{IP_AS_65020-46}";
    peer_port  = 179;

    disconnect = 0;

    # (path prefix B4 specifies BGP4 only update!)
    # <prefix>[,[[B4]? <asn>[p<repetition>]]*[ ]*[I|V|N]?]
    #
    #  Topology as announced by brio_tg using BGP UPDATES:
    #
    #                    <-p2p->
    #                 [F]-------(G)
    #       /|\       /
    #   C2P  |       / 
    #        |      / 
    #              /
    #            (C)       (D)
    #       /|\    \       /  
    #   C2P  |      \     /   
    #        |       \   /    
    #                 \ /     
    #                 (A)     
    #
    #   BRIO: A
    #   IUT:  D
    #
    #   The complete topology is described in the examples README file.
    #
    update = ( 
              "10.65.10.0/22, B4 65030 65040 6510"
              "10.65.14.0/22, B4 65030 65060 65040 55010"
             );

    printOnSend = {
      update       = true;
    };
    
    printSimple    = true;
  }
);

update = ( 
         );

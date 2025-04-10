mode = "BGP";
disable_bgpsec = true;

session = (
  {
    asn        = 65030;
    bgp_ident  = "{IP_AS_65030-13}";
    hold_timer = 180;

    local_addr = "{IP_AS_65030-13}";

    peer_asn   = 65060;
    peer_ip    = "{IP_AS_65060-13}";
    peer_port  = 179;

    disconnect = 0;

    # (path prefix B4 specifies BGP4 only update!)
    # <prefix>[,[[B4]? <asn>[p<repetition>]]*[ ]*[I|V|N]?]
    #
    #  Topology as announced by brio_tg using BGP UPDATES:
    #
    #                65050     65060
    #                 [F]       (G) {AS0 ASPA}
    #       /|\       / \       /   
    #   C2P  |       /   \     /     
    #        |      /     \   /       
    #              /<-p2p->\ /        
    #           65020-----65030
    #            (C)       (D)
    #       /|\    \       
    #   C2P  |      \     
    #        |       \   
    #                 \ 
    #                65000 (A)
    #
    #   BRIO: D
    #   IUT:  G
    #
    #   The complete topology is described in the examples README file.
    #
    update = (  
              "10.65.1.0/22, B4 65020 65000"
              "10.65.2.0/22, B4 65050 65020 65000"
             );

    printOnSend = {
      update       = true;
    };
    
    printSimple    = true;
  }
);

update = ( 
         );

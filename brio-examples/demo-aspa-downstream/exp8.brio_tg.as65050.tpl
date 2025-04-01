mode = "BGP";
disable_bgpsec = true;

session = (
  {
    asn        = 65050;
    bgp_ident  = "{IP_AS_65050-8}";
    hold_timer = 180;

    local_addr = "{IP_AS_65050-8}";

    peer_asn   = 65030;
    peer_ip    = "{IP_AS_65030-8}";
    peer_port  = 179;

    disconnect = 0;

    # (path prefix B4 specifies BGP4 only update!)
    # <prefix>[,[[B4]? <asn>[p<repetition>]]*[ ]*[I|V|N]?]
    #
    #  Topology as announced by brio_tg using BGP UPDATES:
    #
    #                65050
    #                 [F]
    #       /|\       / \ 
    #   C2P  |       /   \ 
    #        |      /     \ 
    #              /       \
    #           65020     65030
    #            (C)       (D)
    #       /|\    \
    #   C2P  |      \   
    #        |       \  
    #                 \ 
    #                65000 (A)
    #
    #   BRIO: A
    #   IUT:  C
    #
    #   The complete topology is described in the examples README file.
    #
    update = (  
              "10.65.0.0/22, B4 65020 65000"
             );

    printOnSend = {
      update       = true;
    };
    
    printSimple    = true;
  }
);

update = ( 
         );

import pysmile

# Tutorial6 creates a dynamic Bayesian network (DBN),
# performs the inference, then saves the model to disk.

class Tutorial6:
    def __init__(self):
        print("Starting Tutorial6...")
        net = pysmile.Network()
        loc = self.create_cpt_node(net,
              "Location", "Location",
              ["Pittsburgh", "Sahara"],
              160, 360)
        rain = self.create_cpt_node(net, 
               "Rain", "Rain", 
               ["true", "false"],
               380, 240)
        umb = self.create_cpt_node(net,
              "Umbrella", "Umbrella",
              ["true", "false"],
              300, 100)
        
        net.set_node_temporal_type(rain, pysmile.NodeTemporalType.PLATE)
        net.set_node_temporal_type(umb, pysmile.NodeTemporalType.PLATE)

        net.add_arc(loc, rain)
        net.add_temporal_arc(rain, rain, 1)
        net.add_arc(rain, umb)

        rain_def = [
            0.7,  # P(Rain=true |Location=Pittsburgh)
            0.3,  # P(Rain=false|Location=Pittsburgh)
            0.01, # P(Rain=true |Location=Sahara)
            0.99  # P(Rain=false|Location=Sahara)
        ]

        net.set_node_definition(rain, rain_def)

        rain_def_temporal = [
            0.7,   # P(Rain=true |Location=Pittsburgh,Rain[t-1]=true)
            0.3,   # P(Rain=false|Location=Pittsburgh,Rain[t-1]=true)
            0.3,   # P(Rain=true |Location=Pittsburgh,Rain[t-1]=false)
            0.7,   # P(Rain=false|Location=Pittsburgh,Rain[t-1]=false)
            0.001, # P(Rain=true |Location=Sahara,Rain[t-1]=true)
            0.999, # P(Rain=false|Location=Sahara,Rain[t-1]=true)
            0.01,  # P(Rain=true |Location=Sahara,Rain[t-1]=false)
            0.99   # P(Rain=false|Location=Sahara,Rain[t-1]=false)
        ]
        net.set_node_temporal_definition(rain, 1, rain_def_temporal)

        umb_def = [
            0.9, # P(Umbrella=true |Rain=true)
            0.1, # P(Umbrella=false|Rain=true)
            0.2, # P(Umbrella=true |Rain=false)
            0.8  # P(Umbrella=false|Rain=false)
        ]
        net.set_node_definition(umb, umb_def)

        net.set_slice_count(5)

        print("Performing update without evidence.")
        self.update_and_show_temporal_results(net)

        print("Setting Umbrella[t=1], to true and Umbrella[t=3] to false.")
        net.set_temporal_evidence(umb, 1, 0)
        net.set_temporal_evidence(umb, 3, 1)
        self.update_and_show_temporal_results(net)

        net.write_file("tutorial6.xdsl")
        print("Tutorial6 complete: Network written to tutorial6.xsdl")

    def update_and_show_temporal_results(self, net):
        net.update_beliefs()
        slice_count = net.get_slice_count()
        for h in net.get_all_nodes():
            if net.get_node_temporal_type(h) == pysmile.NodeTemporalType.PLATE:
                outcome_count = net.get_outcome_count(h)
                print("Temporal beliefs for " + net.get_node_id(h) + ":")
                v = net.get_node_value(h)
                for slice_idx in range(0, slice_count):
                    s = "\tt=" + str(slice_idx) + ":"
                    for i in range(0, outcome_count):
                        s = s + " " + str(v[slice_idx * outcome_count + i])
                    print(s)
        print("")

    def create_cpt_node(self, net, id, name, outcomes, x_pos, y_pos):
        handle = net.add_node(pysmile.NodeType.CPT, id)
        
        net.set_node_name(handle, name)
        net.set_node_position(handle, x_pos, y_pos, 85, 55)
        
        initial_outcome_count = net.get_outcome_count(handle)
        
        for i in range(0, initial_outcome_count):
            net.set_outcome_id(handle, i, outcomes[i])
        
        for i in range(initial_outcome_count, len(outcomes)):
            net.add_outcome(handle, outcomes[i])
            
        return handle

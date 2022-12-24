import pysmile

# Tutorial8 loads continuous model from the XDSL file written by Tutorial7,
# then adds discrete nodes to create a hybrid model. Inference is performed
# and model is saved to disk.

class Tutorial8:
    def __init__(self):
        print("Starting Tutorial8...")
        net = pysmile.Network()

        net.read_file("tutorial7.xdsl")

        self.create_cpt_node(net,
                        "zone", "Climate Zone",
                        ["Temperate", "Desert"],
                        60, 20)
        toa_handle = net.get_node("toa")
        net.set_node_equation("toa", 
                        'toa=If(zone="Desert",Normal(22,5),Normal(11,10))')

        perceived_handle = self.create_cpt_node(net,
                                        "perceived", "Perceived Temperature",
                                        ["Hot", "Warm", "Cold"],
                                        60, 300)
        
        net.add_arc(toa_handle, perceived_handle)

        perceived_probs = [
            0,    # P(perceived=Hot |toa in -10..0)
            0.02, # P(perceived=Warm|toa in -10..0)
            0.98, # P(perceived=Cold|toa in -10..0)
            0.05, # P(perceived=Hot |toa in 0..10)
            0.15, # P(perceived=Warm|toa in 0..10)
            0.80, # P(perceived=Cold|toa in 0..10)
            0.10, # P(perceived=Hot |toa in 10..20)
            0.80, # P(perceived=Warm|toa in 10..20)
            0.10, # P(perceived=Cold|toa in 10..20)
            0.80, # P(perceived=Hot |toa in 20..30)
            0.15, # P(perceived=Warm|toa in 20..30)
            0.05, # P(perceived=Cold|toa in 20..30)
            0.98, # P(perceived=Hot |toa in 30..40)
            0.02, # P(perceived=Warm|toa in 30..40)
            0     # P(perceived=Cold|toa in 30..40)
        ]

        net.set_node_definition(perceived_handle, perceived_probs)

        net.set_evidence("zone", "Temperate")
        print("Results in temperate zone:")
        self.update_and_show_stats(net)

        net.set_evidence("zone", "Desert")
        print("Results in desert zone:")
        self.update_and_show_stats(net)

        net.write_file("tutorial8.xdsl")
        print("Tutorial8 complete: Network written to tutorial8.xdsl")


    def show_stats(self, net, node_handle):
        node_id = net.get_node_id(node_handle)

        if net.is_evidence(node_handle):
            v = net.get_cont_evidence(node_handle)
            print(node_id + " has evidence set " + str(v))
            return

        if net.is_value_discretized(node_handle):
            print(node_id + " is discretized.")
            iv = net.get_node_equation_discretization(node_handle)
            bounds = net.get_node_equation_bounds(node_handle)
            disc_beliefs = net.get_node_value(node_handle)
            lo = bounds[0]
            for i in range(0, len(disc_beliefs)):
                hi = iv[i].boundary
                print("\tP(" + node_id + " in " + str(lo) + ".." + str(hi) 
                      + ")=" + str(disc_beliefs[i]))
                lo = hi
        else:
            stats = net.get_node_sample_stats(node_handle)
            print(node_id + ": mean=" + str(stats[0]) + " stddev=" 
                  + str(stats[1]) + " min=" + str(stats[2]) + " max=" 
                  + str(stats[3]))


    def update_and_show_stats(self, net):
        net.update_beliefs()
        for h in net.get_all_nodes():
            if net.get_node_type(h) == pysmile.NodeType.EQUATION:
                self.show_stats(net, h)

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

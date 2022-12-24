import pysmile

# Tutorial7 creates a network with three equation-based nodes
# performs the inference, then saves the model to disk.

class Tutorial7:
    def __init__(self):
        print("Starting Tutorial7...")
        net = pysmile.Network()

        net.set_outlier_rejection_enabled(True)

        self.create_equation_node(net,
                                  "tra", "Return Air Temperature",
                                  "tra=24", 23.9, 24.1,
                                  280, 100)
                                  
        self.create_equation_node(net,
                                  "u_d", "Damper Control Signal",
                                  "u_d=Bernoulli(0.539)*0.8+0.2", 0, 1,
                                  160, 100)

        toa = self.create_equation_node(net,
                                   "toa", "Outside Air Temperature",
                                   "toa=Normal(11,15)", -10, 40,
                                   60, 100)

        # tra, toa and u_d are referenced in equation
        # arcs are created automatically
        tma = self.create_equation_node(net,
                                   "tma", "Mixed Air Temperature",
                                   "tma=toa*u_d+(tra-tra*u_d)", 10, 30,
                                   110, 200)
        
        self.set_uniform_intervals(net, toa, 5)
        self.set_uniform_intervals(net, tma, 4)

        print("Results with no evidence:")
        self.update_and_show_stats(net)

        net.set_cont_evidence(toa, 28.5)
        print("Results with outside air temperature set to 28.5:")
        self.update_and_show_stats(net)

        net.clear_evidence(toa)
        print("Results with mixed air temperature set to 21:")
        net.set_cont_evidence(tma, 21.0)
        self.update_and_show_stats(net)

        net.write_file("tutorial7.xdsl")
        print("Tutorial7 complete: Network written to tutorial7.xdsl")

    def create_equation_node(self, net, id, name, equation, lo_bound, 
                             hi_bound, x_pos, y_pos):
        handle = net.add_node(pysmile.NodeType.EQUATION, id)
        net.set_node_name(handle, name)
        net.set_node_equation(handle, equation)
        net.set_node_equation_bounds(handle, lo_bound, hi_bound)

        net.set_node_position(handle, x_pos, y_pos, 85, 55)

        return handle

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

    def set_uniform_intervals(self, net, node_handle, count):
        bounds = net.get_node_equation_bounds(node_handle)
        lo = bounds[0]
        hi = bounds[1]

        iv = [None] * count
        for i in range(0, count):
            iv[i] = pysmile.DiscretizationInterval("",lo + (i + 1) * (hi - lo) 
                                                      / count)

        net.set_node_equation_discretization(node_handle, iv)

    def update_and_show_stats(self, net):
        net.update_beliefs()
        for h in net.get_all_nodes():
            self.show_stats(net, h)

        print("")

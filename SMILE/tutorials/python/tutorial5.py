import pysmile

# Tutorial5 loads the XDSL file created by Tutorial4,
# then performs the series of inference calls,
# changing evidence each time.

class Tutorial5:
    def __init__(self):
        print("Starting Tutorial5...")
        net = pysmile.Network()
        
        net.read_file("tutorial4.xdsl")

        print("No evidence set.")
        net.update_beliefs()
        self.print_financial_gain(net)

        print("Setting Forecast=Good.")
        self.change_evidence_and_update(net, "Forecast", "Good")

        print("Adding Economy=Up")
        self.change_evidence_and_update(net, "Economy", "Up")
        
        print("Tutorial5 complete.")	


    def change_evidence_and_update(self, net, node_id, outcome_id):
        if outcome_id is not None:
            net.set_evidence(node_id, outcome_id)	
        else:
            net.clear_evidence(node_id)
        
        net.update_beliefs()
        self.print_financial_gain(net)


    def print_financial_gain(self, net):
        expected_utility = net.get_node_value("Gain")
        util_parents = net.get_value_indexing_parents("Gain")
        self.print_gain_matrix(net, expected_utility, util_parents)


    def print_gain_matrix(self, net, mtx, parents):
        dim_count = 1 + len(parents)
        
        dim_sizes = [0] * dim_count
        for i in range(0, dim_count - 1):
            dim_sizes[i] = net.get_outcome_count(parents[i])
        dim_sizes[len(dim_sizes) - 1] = 1
        coords = [0] * dim_count
        for elem_idx in range(0, len(mtx)):
            self.index_to_coords(elem_idx, dim_sizes, coords)
            str_to_print = "    Utility("
            if dim_count > 1:
                for parent_idx in range(0, len(parents)):
                    if parent_idx > 0:
                        str_to_print += ","
                    parent_handle = parents[parent_idx]
                    str_to_print += net.get_node_id(parent_handle) + \
                    "=" + net.get_outcome_id(parent_handle, 
                                             coords[parent_idx])
            str_to_print += ")=" + str(mtx[elem_idx])
            print(str_to_print)
        print("")


    def index_to_coords(self, index, dim_sizes, coords):
        prod = 1
        for i in range(len(dim_sizes) - 1, -1, -1):
            coords[i] = int(index / prod) % dim_sizes[i]
            prod *= dim_sizes[i]

import pysmile

# Tutorial3 loads the XDSL file and prints the information
# about the structure (nodes and arcs) and the parameters 
# (conditional probabilities of the nodes) of the network.

class Tutorial3:
    def __init__(self):
        print("Starting Tutorial3...")
        net = pysmile.Network()
        
        # load the network created by Tutorial1
        net.read_file("tutorial1.xdsl")
        for h in net.get_all_nodes():
            self.print_node_info(net, h)
        print("Tutorial3 complete.")


    def print_node_info(self, net, node_handle):
        print("Node id/name: " + net.get_node_id(node_handle) + "/" +
              net.get_node_name(node_handle))
        print("  Outcomes: " + " ".join(net.get_outcome_ids(node_handle)))
       
        parent_ids = net.get_parent_ids(node_handle)
        if len(parent_ids) > 0:
            print("  Parents: " + " ".join(parent_ids))
        child_ids = net.get_child_ids(node_handle)
        if len(child_ids) > 0:
            print("  Children: " + " ".join(child_ids))
        
        self.print_cpt_matrix(net, node_handle)


    def print_cpt_matrix(self, net, node_handle):
        cpt = net.get_node_definition(node_handle)
        parents = net.get_parents(node_handle)
        dim_count = 1 + len(parents)
        
        dim_sizes = [0] * dim_count
        for i in range(0, dim_count - 1):
            dim_sizes[i] = net.get_outcome_count(parents[i])
        dim_sizes[len(dim_sizes) - 1] = net.get_outcome_count(node_handle)
        
        coords = [0] * dim_count
        for elem_idx in range(0, len(cpt)):
            self.index_to_coords(elem_idx, dim_sizes, coords)
            
            outcome = net.get_outcome_id(node_handle, coords[dim_count - 1])
            out_str = "    P(" + outcome
            
            if dim_count > 1:
                out_str += " | "
                for parent_idx in range(0, len(parents)):
                    if parent_idx > 0: 
                        out_str += ","
                    parent_handle = parents[parent_idx]
                    out_str += net.get_node_id(parent_handle) + "=" + \
                    net.get_outcome_id(parent_handle, coords[parent_idx])
            
            prob = cpt[elem_idx]
            out_str += ")=" + str(prob)
            print(out_str)


    def index_to_coords(self, index, dim_sizes, coords):
        prod = 1
        for i in range(len(dim_sizes) - 1, -1, -1):
            coords[i] = int(index / prod) % dim_sizes[i]
            prod *= dim_sizes[i]

import pysmile

# Tutorial4 loads the XDSL file file created by Tutorial1
# and adds decision and utility nodes, which transforms
# a Bayesian Network (BN) into an Influence Diagram (ID).

class Tutorial4:
    def __init__(self):
        print("Starting Tutorial4...")
        net = pysmile.Network()
        
        net.read_file("tutorial1.xdsl")

        s = net.get_node("Success")
        i = self.create_node(net, pysmile.NodeType.DECISION, 
            "Invest", "Investment decision", 
            ["Invest", "DoNotInvest"], 160, 240)

        g = self.create_node(net, pysmile.NodeType.UTILITY, 
            "Gain", "Financial gain", None, 60, 200)

        net.add_arc(i, g)
        net.add_arc(s, g)

        gain_definition = [
            10000, # Utility(Invest=I, Success=S)
            -5000, # Utility(Invest=I, Success=F)
            500,   # Utility(Invest=D, Success=S)
            500    # Utility(Invest=D, Success=F)
        ]
        net.set_node_definition(g, gain_definition)
        
        net.write_file("tutorial4.xdsl")

        print("Tutorial4 complete:" +
            " Influence diagram written to tutorial4.xdsl.")


    def create_node(self,
        net, node_type, id, name, 
        outcomes, xPos, yPos):
        handle = net.add_node(node_type, id)
        
        net.set_node_name(handle, name)
        net.set_node_position(handle, xPos, yPos, 85, 55)
        
        if outcomes is not None:
            initial_outcome_count = net.get_outcome_count(handle) 
            for i in range(0, initial_outcome_count):
                net.set_outcome_id(handle, i, outcomes[i])
            
            for i in range(initial_outcome_count, len(outcomes)):
                net.add_outcome(handle, outcomes[i])
            
        return handle

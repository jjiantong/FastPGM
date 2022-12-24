import pysmile

# Tutorial1 creates a simple network with three nodes,
# then writes its content as XDSL file to disk.

class Tutorial1:
    def __init__(self):
        net = pysmile.Network()

        e = self.create_cpt_node(net,
            "Economy", "State of the economy",
            ["Up","Flat","Down"], 
            160, 40)
        
        s = self.create_cpt_node(net,
            "Success", "Success of the venture",
            ["Success","Failure"], 
            60, 40)
        
        f = self.create_cpt_node(net,
            "Forecast", "Expert forecast",
            ["Good","Moderate","Poor"], 
            110, 140)
            
        net.add_arc(e, s)
        net.add_arc(s, f)
        
        # we can also use node identifiers when creating arcs  
        net.add_arc("Economy", "Forecast");
        
        economyDef = [
            0.2, # P(Economy=U)
            0.7, # P(Economy=F)
            0.1  # P(Economy=D)
        ]
        net.set_node_definition(e, economyDef)
        
        successDef = [
            0.3, # P(Success=S|Economy=U)
            0.7, # P(Success=F|Economy=U)
            0.2, # P(Success=S|Economy=F)
            0.8, # P(Success=F|Economy=F)
            0.1, # P(Success=S|Economy=D)
            0.9  # P(Success=F|Economy=D)
        ]
        net.set_node_definition(s, successDef)
        
        forecastDef = [
            0.70, # P(Forecast=G|Success=S,Economy=U)
            0.29, # P(Forecast=M|Success=S,Economy=U)
            0.01, # P(Forecast=P|Success=S,Economy=U)
        
            0.65, # P(Forecast=G|Success=S,Economy=F)
            0.30, # P(Forecast=M|Success=S,Economy=F)
            0.05, # P(Forecast=P|Success=S,Economy=F)
        
            0.60, # P(Forecast=G|Success=S,Economy=D)
            0.30, # P(Forecast=M|Success=S,Economy=D)
            0.10, # P(Forecast=P|Success=S,Economy=D)
        
            0.15,  # P(Forecast=G|Success=F,Economy=U)
            0.30, # P(Forecast=M|Success=F,Economy=U)
            0.55, # P(Forecast=P|Success=F,Economy=U)
        
            0.10, # P(Forecast=G|Success=F,Economy=F)
            0.30, # P(Forecast=M|Success=F,Economy=F)
            0.60, # P(Forecast=P|Success=F,Economy=F)
        
            0.05, # P(Forecast=G|Success=F,Economy=D)
            0.25, # P(Forecast=G|Success=F,Economy=D)
            0.70  # P(Forecast=G|Success=F,Economy=D)
        ]
        net.set_node_definition(f, forecastDef)
        
        net.write_file("tutorial1.xdsl")
        
        print("Tutorial1 complete: Network written to tutorial1.xdsl")


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

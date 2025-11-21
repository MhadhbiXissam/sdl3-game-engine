from typing import Type, Dict, List, Optional, Any
from dataclasses import dataclass, field

#=============================
# Component Base
#=============================
class Component:
    """Base class for all components."""
    pass

# Example components
@dataclass
class Transform(Component):
    x: float = 0.0
    y: float = 0.0
    z: float = 0.0
    rotation: float = 0.0
    scale: float = 1.0

@dataclass
class Renderable(Component):
    mesh: str = ""  # path or identifier
    color: tuple = (1.0, 1.0, 1.0)

@dataclass
class PhysicsBody(Component):
    velocity: tuple = (0.0, 0.0, 0.0)
    mass: float = 1.0

#=============================
# Entity Node
#=============================
class Node:
    def __init__(self, name: str):
        self.name = name
        self.children: List["Node"] = []
        self.components: Dict[Type[Component], Component] = {}
        self.parent: Optional["Node"] = None

    # Add a component to the node
    def add_component(self, component: Component):
        self.components[type(component)] = component
        return component

    # Get a component from the node
    def get_component(self, comp_type: Type[Component]) -> Optional[Component]:
        return self.components.get(comp_type, None)

    # Add a child node
    def add_child(self, child: "Node"):
        child.parent = self
        self.children.append(child)

    # Remove a child node
    def remove_child(self, child: "Node"):
        if child in self.children:
            child.parent = None
            self.children.remove(child)

    # Traverse tree recursively
    def traverse(self, action):
        action(self)
        for child in self.children:
            child.traverse(action)

#=============================
# ECS System Base
#=============================
class System:
    def update(self, nodes: List[Node], dt: float):
        """Update system logic for all relevant nodes."""
        raise NotImplementedError()

# Example system: physics
class PhysicsSystem(System):
    def update(self, nodes: List[Node], dt: float):
        def apply_physics(node: Node):
            transform: Transform = node.get_component(Transform)
            physics: PhysicsBody = node.get_component(PhysicsBody)
            if transform and physics:
                x, y, z = transform.x, transform.y, transform.z
                vx, vy, vz = physics.velocity
                transform.x += vx * dt
                transform.y += vy * dt
                transform.z += vz * dt

        for node in nodes:
            node.traverse(apply_physics)

# Example system: rendering (placeholder)
class RenderSystem(System):
    def update(self, nodes: List[Node], dt: float):
        def render(node: Node):
            transform: Transform = node.get_component(Transform)
            renderable: Renderable = node.get_component(Renderable)
            if transform and renderable:
                # Here you would call OpenGL or other renderer
                print(f"Render {node.name} at ({transform.x:.2f},{transform.y:.2f}) color={renderable.color}")

        for node in nodes:
            node.traverse(render)

#=============================
# Example Usage
#=============================
if __name__ == "__main__":
    # Create root node
    root = Node("root")

    # Create player node
    player = Node("player")
    player.add_component(Transform(x=0, y=0))
    player.add_component(Renderable(mesh="player.obj", color=(1,0,0)))
    player.add_component(PhysicsBody(velocity=(1,0,0)))
    root.add_child(player)

    # Create enemy node
    enemy = Node("enemy")
    enemy.add_component(Transform(x=5, y=5))
    enemy.add_component(Renderable(mesh="enemy.obj", color=(0,1,0)))
    root.add_child(enemy)

    # Systems
    physics_system = PhysicsSystem()
    render_system = RenderSystem()

    # Simulation loop
    dt = 1/60
    for frame in range(3000):
        print(f"--- Frame {frame} ---")
        physics_system.update([root], dt)
        render_system.update([root], dt)

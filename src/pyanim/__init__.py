from __future__ import absolute_import
from __future__ import division
from __future__ import print_function
import os

import pygraphviz as pgv

class Command(object):
    def __call__(self, graph):
        pass

    @classmethod
    def from_string(cls, string):
        tokens = string.strip().split(' ')
        return COMMANDS[tokens[0]].from_tokens(tokens[1:])

class NewFrameCommand(Command):
    @classmethod
    def from_tokens(cls, tokens):
        return cls()

class InitialCommand(Command):
    def __init__(self, num_nodes):
        self.num_nodes = num_nodes

    def __call__(self, graph):
        for node in xrange(self.num_nodes):
            graph.add_node(node)

    @classmethod
    def from_tokens(cls, tokens):
        return cls(int(tokens[0]))


class RandomLinkCommand(Command):
    def __init__(self, i, j):
        self.i = i
        self.j = j

    def __call__(self, graph):
        graph.add_edge(self.i, self.j)

    @classmethod
    def from_tokens(cls, tokens):
        return cls(*map(int, tokens))


class AddNodeCommand(Command):
    def __call__(self, graph):
        graph.add_node(graph.number_of_nodes())

    @classmethod
    def from_tokens(cls, tokens):
        return cls()


class AddLinkCommand(Command):
    def __init__(self, i, j):
        self.i = i
        self.j = j

    def __call__(self, graph):
        graph.add_edge(self.i, self.j)

    @classmethod
    def from_tokens(cls, tokens):
        return cls(*map(int, tokens))


class SetNodeColour(Command):
    def __init__(self, i, colour):
        self.i = i
        self.colour = colour

    def __call__(self, graph):
        node = graph.get_node(self.i)
        node.attr['fillcolor'] = self.colour

    @classmethod
    def from_tokens(cls, tokens):
        return cls(int(tokens[0]), tokens[1])

class SetEdgeColour(Command):
    def __init__(self, i, j, colour):
        self.i = i
        self.j = j
        self.colour = colour

    def __call__(self, graph):
        edge = graph.get_edge(self.i, self.j)
        edge.attr['color'] = self.colour

    @classmethod
    def from_tokens(cls, tokens):
        return cls(int(tokens[0]), int(tokens[1]), tokens[2])


COMMANDS = {
    'F': NewFrameCommand,
    'I': InitialCommand,
    'R': RandomLinkCommand,
    'A': AddNodeCommand,
    'L': AddLinkCommand,
    'C': SetNodeColour,
    'E': SetEdgeColour,
}

class Animation:
    def __init__(self, commands):
        self.commands = commands

    def __call__(self):
        graph = pgv.AGraph()
        graph.node_attr.update(style='filled', fillcolor='white')
        frames = []
        for command in self.commands:
            if isinstance(command, NewFrameCommand):
                frames.append(graph)
                graph = graph.copy()
            else:
                command(graph)
        return frames

    @classmethod
    def from_file(cls, path):
        with open(path) as animation_file:
            commands = []
            for command in animation_file:
                commands.append(Command.from_string(command))
        return cls(commands)

class Frames(object):
    def __init__(self, frames):
        self.frames = frames

    def __call__(self, dirpath, format='png', prog='dot'):
        for i, frame in enumerate(self.frames):
            frame.layout(prog=prog)
            frame.draw(path=os.path.join(dirpath, '%d.%s' % (i, format)),
                       format=format)

    @classmethod
    def from_file(self, path):
        animation = Animation.from_file(path)
        return Frames(animation())


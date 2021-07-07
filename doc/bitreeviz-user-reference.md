
# bitreeviz user reference  
version 0.1.0

- [bitreeviz user reference](#bitreeviz-user-reference)
  - [Grammar of bitree.](#grammar-of-bitree)
  - [Property table.](#property-table)
  - [NodeId](#nodeid)
  - [Property](#property)
    - [Node scope property](#node-scope-property)
    - [Global scope property](#global-scope-property)
  - [NodeLabel](#nodelabel)
  - [NodeRadius](#noderadius)
  - [Color](#color)
  - [Dash](#dash)
  - [FontFamily and FontSlant and FontWeight](#fontfamily-and-fontslant-and-fontweight)
  - [References.](#references)

## Grammar of bitree.

```
BiTree          ::= StartSymbol tree NewLine {PropertyList}*
tree            ::= NodeId "(" Node "," Node ")" | NodeId
NodeId          ::= Uint32
Node            ::= tree | Epsilon
StartSymbol     ::= "bitree = "
Uint32          ::= [0-9]+
NewLine         ::= [\n]
Epsilon         ==> it means nothing or empty

PropertyList    ::= [Property]+
Property        ::= {NodeId} "[" PropertyRecord {"," PropertyRecord}* "]" NewLine
PropertyRecord  ::= PropertyName "=" PropertyValue 
PropertyName    ::= [a-zA-Z0-9_]+
PropertyValue   ::= [0-9]+
                | "0x" [0-9A-F]+
                | [0-9a-zA-Z_]+
```

## Property table.

| NO   | Property-name        | Type     | Min  | Max        | Default    | Scope       |
| ---- | -------------------- | -------- | ---- | ---------- | ---------- | ----------- |
| 1    | NodeLabel            | string   |      |            |            | Node+Global |
| 2    | NodeRadius           | uint32_t | 0    | 255        | auto       | Node+Global |
| 3    | BorderColor          | uint32_t | 0    | 0xffffffff | 0x000000ff | Node+Global |
| 4    | BorderWidth          | uint32_t | 0    | 12         | 1          | Node+Global |
| 5    | BorderDash           | string   |      |            |            | Node+Global |
| 6    | FillColor            | uint32_t | 0    | 0xffffffff | 0          | Node+Global |
| 7    | EdgeColor            | uint32_t | 0    | 0xffffffff | 0x000000ff | Node+Global |
| 8    | EdgeWidth            | uint32_t | 0    | 12         | 1          | Node+Global |
| 9    | EdgeDash             | string   | 0    |            |            | Node+Global |
| 10   | FontColor            | uint32_t | 0    | 0xffffffff | 0x000000ff | Node+Global |
| 11   | FontFamily           | string   |      |            |            | Global      |
| 12   | FontSize             | uint32_t | 0    | 36         | 12         | Global      |
| 13   | FontSlant            | uint32_t | 0    | 1          | 0          | Global      |
| 14   | FontWeight           | uint32_t | 0    | 1          | 0          | Global      |
| 15   | RadiusIncrement      | uint32_t | 0    | 255        | 0          | Global      |
| 16   | EdgeXIncrement       | uint32_t | 0    | 255        | 0          | Global      |
| 17   | EdgeYIncrement       | uint32_t | 0    | 255        | 0          | Global      |
| 18   | PageBackgroundColor  | uint32_t | 0    | 0xffffffff | 32         | Global      |
| 19   | PageMarginXIncrement | uint32_t | 0    | 255        | 0          | Global      |
| 20   | PageMarginYIncrement | uint32_t | 0    | 255        | 0          | Global      |
|      |                      |          |      |            |            |             |

## NodeId 

Every node should using a unique NodeId, if some nodes using the same NodeId, these nodes will share the same properties.

## Property
### Node scope property
Node scope property will only apply to node(s) with the NodeId, the format for using is:  

**NodeId [PropertyName = PropertyValue]**

### Global scope property
Global property will apply to all node(s) in the bitree, the format for using is:  

 **[PropertyName = PropertyValue]**

If a node add same properties both in global-scope and node-scope, the node-scope property will finally take effect. if a node add same properties many times, only the last one will take effect.

## NodeLabel

If a node do not add a NodeLabel property, the default nodeLabel is string of its nodeId, in this case, the nodeId can be a string ([0-9a-zA-Z_]+).

## NodeRadius

bitreeviz will give out default radius for all the nodes of the tree, If it is too small some times, you can add global RadiusIncrement property to increase its radius.

There is only one shape(circle) for all the nodes, other shapes are not supported currently.

## Color
The color value format is 0xRRGGBBAA.

## Dash

Format of BorderDash and EdgeDash is a string, the format is:
```
DashString    ::= [DoubleQuote] [DashUnit SpaceChar DashUnit]  {[SpaceChar DashUnit]}* [DoubleQuote]
DoubleQuote   ::= ["]
SpaceChar     ::= [ox20]+
DashUnit      ::= [0-9]+
```


## FontFamily and FontSlant and FontWeight

These properties may do not work, if you want change any of them, you'd better using these combined properties, such as:

```
[FontFamily="your font name", FontWeight=0,FontSlant=0]

[FontFamily="your font name", FontWeight=0,FontSlant=1]

[FontFamily="your font name", FontWeight=1,FontSlant=0]

[FontFamily="your font name", FontWeight=1,FontSlant=1]
```
## References.
https://www.cairographics.org/  
https://reingold.co/graph-drawing.shtml  
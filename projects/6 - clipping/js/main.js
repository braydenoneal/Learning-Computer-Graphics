const canvas = document.createElement('canvas')
document.body.appendChild(canvas)
const context = canvas.getContext('2d')

context.canvas.width = window.innerWidth
context.canvas.height = window.innerHeight

let screen_width = canvas.width
let screen_height = canvas.height

function subtract(a, b) {
    let c = []

    for (let row = 0; row < a.length; row++) {
        let c_row = []

        for (let column = 0; column < a[0].length; column++) {
            c_row.push(a[row][column] - b[row][column])
        }

        c.push(c_row)
    }

    return c
}

function multiply(a, b) {
    let c = []

    for (let a_row = 0; a_row < a.length; a_row++) {
        let c_row = []

        for (let b_column = 0; b_column < b[0].length; b_column++) {
            let c_row_sum = 0

            for (let a_column = 0; a_column < a[0].length; a_column++) {
                c_row_sum += a[a_row][a_column] * b[a_column][b_column]
            }

            c_row.push(c_row_sum)
        }

        c.push(c_row)
    }

    return c
}

function project(vertex, camera_vertex, camera_angle) {
    let first = [
        [1, 0, 0],
        [0, Math.cos(camera_angle[0][0]), Math.sin(camera_angle[0][0])],
        [0, -1 * Math.sin(camera_angle[0][0]), Math.cos(camera_angle[0][0])]
    ]
    let second = [
        [Math.cos(camera_angle[1][0]), 0, -1 * Math.sin(camera_angle[1][0])],
        [0, 1, 0],
        [Math.sin(camera_angle[1][0]), 0, Math.cos(camera_angle[1][0])]
    ]
    let third = [
        [Math.cos(camera_angle[2][0]), Math.sin(camera_angle[2][0]), 0],
        [-1 * Math.sin(camera_angle[2][0]), Math.cos(camera_angle[2][0]), 0],
        [0, 0, 1]
    ]
    let fourth = subtract(vertex, camera_vertex)

    return multiply(multiply(first, second), multiply(third, fourth))
}

function f(x, from, to) {
    let a = from[0][0]
    let b = from[1][0]
    let c = from[2][0]
    let d = to[0][0] - a
    let e = to[1][0] - b
    let f = to[2][0] - c

    let x_value = (x - a) / d
    if (d === 0) {
        x_value = 1
    }
    let y = x_value * e + b
    let z = x_value * f + c

    return [[x], [y], [z]]
}

function fy(y, from, to) {
    let a = from[0][0]
    let b = from[1][0]
    let c = from[2][0]
    let d = to[0][0] - a
    let e = to[1][0] - b
    let f = to[2][0] - c

    let y_value = (y - b) / e
    if (d === 0) {
        y_value = 0
    }
    let x = y_value * d + a
    let z = y_value * f + c

    return [[x], [y], [z]]
}

function fz(z, from, to) {
    let a = from[0][0]
    let b = from[1][0]
    let c = from[2][0]
    let d = to[0][0] - a
    let e = to[1][0] - b
    let f = to[2][0] - c

    let z_value = (z - c) / f
    if (d === 0) {
        z_value = 0
    }
    let x = z_value * d + a
    let y = z_value * e + b

    return [[x], [y], [z]]
}

// console.log(f(
//     [],
//     [],
// ))

function line_value_3d(from_vertex, to_vertex, axis, value) {
    let other_axes = [0, 1, 2]
    other_axes.splice(axis, 1)

    let new_vertex = []

    if (axis === 0) {
        new_vertex = f(value, from_vertex, to_vertex)
    } else  if (axis === 1) {
        new_vertex = fy(value, from_vertex, to_vertex)
    } else if (axis === 2) {
        new_vertex = fz(value, from_vertex, to_vertex)
    }

        // vertex[axis] = new_vertex[0]
    // vertex[other_axes[0]] = new_vertex[1]
    // vertex[other_axes[1]] = new_vertex[2]

    console.log(from_vertex)
    console.log(to_vertex)
    console.log(axis)
    console.log(value)
    console.log(new_vertex)

    return new_vertex
}

let camera_vertex = [[1.8], [0.65], [-1.2]]
let camera_angle = [[-0.0], [-0.0], [0.0]]
let field_of_view_degrees = 70

let tris = [
    [[0.0, 1.0, 1.0, [255, 0, 0]], [1.0, 0.0, 1.0, [0, 255, 0]], [1.0, 1.0, 1.0, [0, 0, 255]]],
    [[0.0, 0.0, 1.0, [255, 0, 0]], [0.0, 1.0, 1.0, [0, 255, 0]], [1.0, 0.0, 1.0, [0, 0, 255]]],
    [[0.0, 1.0, 1.0, [255, 0, 0]], [1.0, 1.0, 0.0, [0, 255, 0]], [1.0, 1.0, 1.0, [0, 0, 255]]],
    [[0.0, 1.0, 0.0, [255, 0, 0]], [0.0, 1.0, 1.0, [0, 255, 0]], [1.0, 1.0, 0.0, [0, 0, 255]]],
    [[1.0, 0.0, 1.0, [255, 0, 0]], [1.0, 1.0, 0.0, [0, 255, 0]], [1.0, 1.0, 1.0, [0, 0, 255]]],
    [[1.0, 0.0, 0.0, [255, 0, 0]], [1.0, 0.0, 1.0, [0, 255, 0]], [1.0, 1.0, 0.0, [0, 0, 255]]],
    [[0.0, 1.0, 0.0, [255, 0, 0]], [1.0, 0.0, 0.0, [0, 255, 0]], [1.0, 1.0, 0.0, [0, 0, 255]]],
    [[0.0, 0.0, 0.0, [255, 0, 0]], [0.0, 1.0, 0.0, [0, 255, 0]], [1.0, 0.0, 0.0, [0, 0, 255]]],
    [[0.0, 0.0, 1.0, [255, 0, 0]], [1.0, 0.0, 0.0, [0, 255, 0]], [1.0, 0.0, 1.0, [0, 0, 255]]],
    [[0.0, 0.0, 0.0, [255, 0, 0]], [0.0, 0.0, 1.0, [0, 255, 0]], [1.0, 0.0, 0.0, [0, 0, 255]]],
    [[0.0, 0.0, 1.0, [255, 0, 0]], [0.0, 1.0, 0.0, [0, 255, 0]], [0.0, 1.0, 1.0, [0, 0, 255]]],
    [[0.0, 0.0, 0.0, [255, 0, 0]], [0.0, 0.0, 1.0, [0, 255, 0]], [0.0, 1.0, 0.0, [0, 0, 255]]]
]

let projected_tris = []

for (let tri of tris) {
    let projected_tri = []

    for(let vertex of tri) {
        projected_tri.push(project([[vertex[0]], [vertex[1]], [vertex[2]]], camera_vertex, camera_angle))
    }

    projected_tris.push(projected_tri)
}

let positive_x_clip = 1
let negative_x_clip = -1

let positive_y_clip = 1
let negative_y_clip = -1

let positive_z_clip = 1000
let negative_z_clip = 0.001

let clipped_tris = []

for (let projected_tri of projected_tris) {
    /*
    01
        -
            point0: <0(x),1(y),2(z)>
            point1: <0(x),1(y),2(z)>
        +
            point0: <0(x),1(y),2(z)>
            point1: <0(x),1(y),2(z)>

    12
        -
            point0: <0(x),1(y),2(z)>
            point1: <0(x),1(y),2(z)>
        +
            point0: <0(x),1(y),2(z)>
            point1: <0(x),1(y),2(z)>

    20
        -
            point0: <0(x),1(y),2(z)>
            point1: <0(x),1(y),2(z)>
        +
            point0: <0(x),1(y),2(z)>
            point1: <0(x),1(y),2(z)>
     */
    let clips = [[[-1, -1], [-1, -1]], [[-1, -1], [-1, -1]], [[-1, -1], [-1, -1]]]

    let removed_vertices = [0, 0, 0]

    let lines = [[0, 1], [1, 2], [2, 0]]
    let axes = [0, 1, 2]
    let points = [0, 1]

    let negative_clips = [negative_x_clip, negative_y_clip, negative_z_clip]
    let positive_clips = [positive_x_clip, positive_y_clip, positive_z_clip]

    let all_clips = [negative_clips, positive_clips]

    for (let line of lines) {
        for (let axis of axes) {
            for (let point of points) {
                let scale = axis === 2 ? 1 : projected_tri[line[point]][2][0]
                if (projected_tri[line[point]][axis][0] > positive_clips[axis] * scale) {
                    clips[lines.indexOf(line)][1][point] = axis
                    removed_vertices[line[point]] = 1
                } else if (projected_tri[line[point]][axis][0] < negative_clips[axis] * scale) {
                    console.log(projected_tri[line[point]])
                    clips[lines.indexOf(line)][0][point] = axis
                    removed_vertices[line[point]] = 1
                }
            }
        }
    }

    console.log(clips)

    // All vertices outside if at least one axis is all positive or negative for all three vertices

    let new_vertices = []

    for (let line = 0; line < clips.length; line++) {
        let line_vertices = [projected_tri[lines[line][0]], projected_tri[lines[line][1]]]

        for (let polarity = 0; polarity < clips[0].length; polarity++) {
            for (let point = 0; point < clips[0][0].length; point++) {
                let axis = clips[line][polarity][point]

                if (axis !== -1) {
                    let value = all_clips[polarity][axis]

                    new_vertices.push(line_value_3d(line_vertices[0], line_vertices[1], axis, value))
                }
            }
        }
    }

    for (let vertex = 0; vertex < projected_tri.length; vertex++) {
        if (removed_vertices[vertex] === 0) {
            new_vertices.unshift(projected_tri[vertex])
        }
    }

    let new_tris = []

    for (let i = 0; i < new_vertices.length - 2; i++) {
        new_tris.push([
            new_vertices[0],
            new_vertices[i + 1],
            new_vertices[i + 2]
        ])
    }

    if (new_vertices.length === 3) {
        clipped_tris.push(new_vertices)
    } else {
        for (let tri of new_tris) {
            clipped_tris.push(tri)
        }
    }
}

for (let tri of clipped_tris) {
    let points = []

    for (let vertex of tri) {
        let display_surface_distance = 1 / (Math.tan(field_of_view_degrees / 2))

        let x = (display_surface_distance * vertex[0][0] / vertex[2][0]) * 800 / 2 + screen_width / 2
        let y = (display_surface_distance * vertex[1][0] / vertex[2][0]) * -1 * 800 / 2 + 800 / 2

        // if (x === -Infinity) {
        //     x = 0
        //     y = screen_height
        // }

        points.push([x, y])
    }

    let x1 = points[0][0]
    let y1 = points[0][1]
    let x2 = points[1][0]
    let y2 = points[1][1]
    let x3 = points[2][0]
    let y3 = points[2][1]

    context.beginPath()
    context.moveTo(x1, y1)
    context.lineTo(x2, y2)
    context.lineTo(x3, y3)
    context.lineTo(x1, y1)
    context.strokeStyle = 'white'
    context.lineWidth = 3
    context.stroke()

    // context.fillStyle = 'rgba(255, 0, 0, 255)'
    // context.fillRect(x1, y1, 16, 16)
    // context.fillRect(x2, y2, 16, 16)
    // context.fillRect(x3, y3, 16, 16)
}

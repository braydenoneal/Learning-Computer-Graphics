// Classes
class Matrix {
    data

    constructor(data) {
        if (!(data[0] instanceof Array)) {
            let new_data = []
            for (let value of data) {
                new_data.push([value])
            }
            this.data = new_data
        } else {
            this.data = data
        }
    }

    size = (index) => {
        return [this.row_quantity(), this.column_quantity()][index]
    }

    row_quantity = () => {
        return this.data.length
    }

    column_quantity = () => {
        return this.get_row(0).length
    }

    get_row = (index) => {
        return this.data[index]
    }

    get = (row_index, column_index = 0) => {
        return this.data[row_index][column_index]
    }

    set_row = (index, data) => {
        this.data[index] = data
    }

    set = (row_index, column_index, data) => {
        this.data[row_index][column_index] = data
    }

    append_row = (data) => {
        this.data.push(data)
    }

    static add = (first, second) => {
        let result = new Matrix([])

        for (let row = 0; row < first.size(0); row++) {
            let result_row = []

            for (let column = 0; column < first.size(1); column++) {
                result_row.push(first.get(row, column) + second.get(row, column))
            }

            result.append_row(result_row)
        }

        return result
    }

    static subtract = (first, second) => {
        let result = new Matrix([])

        for (let row = 0; row < first.size(0); row++) {
            let result_row = []

            for (let column = 0; column < first.size(1); column++) {
                result_row.push(first.get(row, column) - second.get(row, column))
            }

            result.append_row(result_row)
        }

        return result
    }

    static multiply = (first, second) => {
        let result = new Matrix([])

        for (let first_row = 0; first_row < first.size(0); first_row++) {
            let result_row = []

            for (let second_column = 0; second_column < second.size(1); second_column++) {
                let sum = 0

                for (let first_column = 0; first_column < first.size(0); first_column++) {
                    sum += first.get(first_row, first_column) * second.get(first_column, second_column)
                }

                result_row.push(sum)
            }

            result.append_row(result_row)
        }


        return result
    }

    static of = (data) => {
        return new Matrix(data)
    }
}

class VertexProjected {
    constructor(x, y, depth) {
        this._x = x
        this._y = y
        this._depth = depth
    }

    get x() {
        return this._x
    }

    get y() {
        return this._y
    }

    get depth() {
        return this._depth
    }

    static of = (x, y, depth) => {
        return new VertexProjected(x, y, depth)
    }
}

class Tri {
    vertex_0
    vertex_1
    vertex_2

    constructor(vertex_0, vertex_1, vertex_2) {
        this.vertex_0 = vertex_0
        this.vertex_1 = vertex_1
        this.vertex_2 = vertex_2
    }

    static of = (vertex_0, vertex_1, vertex_2) => {
        return new Tri(vertex_0, vertex_1, vertex_2)
    }
}

// Math
const transform = (vertex_position, origin_position, input_viewing_angle) => {
    let viewing_angle = Matrix.of(input_viewing_angle)

    return Matrix.multiply(
        Matrix.multiply(
            Matrix.of([[1, 0, 0],
             [0, Math.cos(viewing_angle.get(0)), Math.sin(viewing_angle.get(0))],
             [0, -1 * Math.sin(viewing_angle.get(0)), Math.cos(viewing_angle.get(0))]]),
            Matrix.of([[Math.cos(viewing_angle.get(1)), 0, -1 * Math.sin(viewing_angle.get(1))],
             [0, 1, 0],
             [Math.sin(viewing_angle.get(1)), 0, Math.cos(viewing_angle.get(1))]])
        ),
        Matrix.multiply(
            Matrix.of([[Math.cos(viewing_angle.get(2)), Math.sin(viewing_angle.get(2)), 0],
             [-1 * Math.sin(viewing_angle.get(2)), Math.cos(viewing_angle.get(2)), 0],
             [0, 0, 1]]),
            Matrix.subtract(Matrix.of(vertex_position), Matrix.of(origin_position))
        )
    ).data
}

const project = (vertex, field_of_view_degrees) => {
    let matrix = Matrix.of(vertex)

    // if (matrix.get(2) < 0) {
    //     return NaN
    // }

    let display_surface_distance = 1 / (Math.tan(field_of_view_degrees / 2))

    let x = display_surface_distance * matrix.get(0) / matrix.get(2)
    let y = display_surface_distance * matrix.get(1) / matrix.get(2)

    return [x, y, matrix.get(2)]
}

// Canvas
const vertex_to_vertex_projected = (input_vertex, screen_width, screen_height) => {
    let vertex = Matrix.of(input_vertex)

    return VertexProjected.of(
        vertex.get(0) * screen_height / 2 + screen_width / 2,
        -1 * vertex.get(1) * screen_height / 2 + screen_height / 2,
        vertex.get(2)
    )
}

const draw_tri = (tri, context, screen_width, screen_height, position, angle, fov) => {
    let vertex_0_projected = vertex_to_vertex_projected(project(transform(tri.vertex_0, position, angle), fov), screen_width, screen_height)
    let vertex_1_projected = vertex_to_vertex_projected(project(transform(tri.vertex_1, position, angle), fov), screen_width, screen_height)
    let vertex_2_projected = vertex_to_vertex_projected(project(transform(tri.vertex_2, position, angle), fov), screen_width, screen_height)

    context.beginPath()
    context.moveTo(vertex_0_projected.x, vertex_0_projected.y)
    context.lineTo(vertex_1_projected.x, vertex_1_projected.y)
    context.lineTo(vertex_2_projected.x, vertex_2_projected.y)
    context.fillStyle = `rgba(255, 0, 0, 255)`
    context.fill()
}

const draw_tris = (tris, context, screen_width, screen_height, position, angle, fov) => {
    for (let tri of tris) {
        draw_tri(tri, context, screen_width, screen_height, position, angle, fov)
    }
}

const tris_from_data = (data) => {
    let tris = []

    for (let tri_data of data) {
        tris.push(Tri.of(
            tri_data[0],
            tri_data[1],
            tri_data[2]
        ))
    }

    return tris
}

// Test Setup
const canvas = document.createElement('canvas')
document.body.appendChild(canvas)
const context = canvas.getContext('2d')

context.canvas.width = window.innerWidth
context.canvas.height = window.innerHeight

const clear_screen = (context, screen_width, screen_height) => {
    context.fillStyle = `rgba(0, 0, 0, 255)`
    context.fillRect(0, 0, screen_width, screen_height)
}

import json from './teapot.json' assert { type: 'json' }

const teapot = tris_from_data(json.tris)

let position = [0,2,-10]
let angle = [0,0,0]
let fov = 70

let x = 0
let y = 0

document.onmousemove = (event) => {
    if (document.pointerLockElement === canvas) {
        x += event.movementX * 2.0
        y += event.movementY * 2.0
        if (y / canvas.height < -0.5 * Math.PI) {
            y = 0.5 * Math.PI * canvas.height
        }
        if (y / canvas.height > 0.5 * Math.PI) {
            y = 0.5 * Math.PI * canvas.height
        }
        angle[0] = y / canvas.height
        angle[1] = x / canvas.width
    }
}

canvas.onclick = () => canvas.requestPointerLock()

const update = () => {
    clear_screen(context, canvas.width, canvas.height)

    draw_tris(teapot, context, canvas.width, canvas.height, position, angle, fov)

    requestAnimationFrame(update)
}

update()

const canvas = document.createElement('canvas')
document.body.appendChild(canvas)
const context = canvas.getContext('2d')

context.canvas.width = window.innerWidth
context.canvas.height = window.innerHeight

let scale = 4

let screen_width = Math.floor(canvas.width / scale)
let screen_height = Math.floor(canvas.height / scale)

context.imageSmoothingEnabled = false

class Tri {
    constructor(vertex_0, vertex_1, vertex_2, texture) {
        this.vertex_0 = vertex_0
        this.vertex_1 = vertex_1
        this.vertex_2 = vertex_2
        this.texture = texture
    }

    static of(...tris_data) {
        let tris = []

        for (let tri of tris_data) {
           tris.push(new Tri(...tri))
        }

        return tris
    }
}

class Vertex {
    constructor(x, y, z, u, v, b) {
        this.x = x
        this.y = y
        this.z = z
        this.u = u
        this.v = v
        this.b = b
    }

    m() {
        return [[this.x], [this.y], [this.z]]
    }

    static of(x, y, z, u, v, b) {
        return new Vertex(x, y, z, u, v, b)
    }
}

class ProjectedVertex {
    constructor(x, y, z, u, v, b) {
        this.x = x
        this.y = y
        this.z = z
        this.u = u
        this.v = v
        this.b = b
    }

    static from(x, y, v) {
        return new ProjectedVertex(x, y, v.z, v.u, v.v, v.b)
    }
}

function subtract_matrix(a, b) {
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

function multiply_matrix(a, b) {
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

function project_matrix(project_vertex, camera_vertex, angle_vertex, field_of_view_degrees, screen_width, screen_height) {
    let first = [
        [1, 0, 0],
        [0, Math.cos(angle_vertex[0][0]), Math.sin(angle_vertex[0][0])],
        [0, -1 * Math.sin(angle_vertex[0][0]), Math.cos(angle_vertex[0][0])]
    ]
    let second = [
        [Math.cos(angle_vertex[1][0]), 0, -1 * Math.sin(angle_vertex[1][0])],
        [0, 1, 0],
        [Math.sin(angle_vertex[1][0]), 0, Math.cos(angle_vertex[1][0])]
    ]
    let third = [
        [Math.cos(angle_vertex[2][0]), Math.sin(angle_vertex[2][0]), 0],
        [-1 * Math.sin(angle_vertex[2][0]), Math.cos(angle_vertex[2][0]), 0],
        [0, 0, 1]
    ]
    let fourth = subtract_matrix(project_vertex, camera_vertex)

    let d = multiply_matrix(multiply_matrix(first, second), multiply_matrix(third, fourth))

    let display_surface_distance = 1 / (Math.tan(field_of_view_degrees / 2))
    if (d[2][0] > 0) {
        let x = (display_surface_distance * d[0][0] / d[2][0]) * screen_height / 2 + screen_width / 2
        let y = (display_surface_distance * d[1][0] / d[2][0]) * -1 * screen_height / 2 + screen_height / 2

        return [Math.floor(x), Math.floor(y), d[2][0]]
    } else {
        return NaN
    }
}

function edge(px, py, v0x, v0y, v1x, v1y) {
    return (v1x - px) * (v0y - py) - (v1y - py) * (v0x - px)
}

let tris_data = Tri.of(
    [Vertex.of( 0.5, -0.5, -0.5, 1, 0, 191), Vertex.of(-0.5, -0.5, -0.5, 0, 0, 191), Vertex.of(-0.5,  0.5, -0.5, 0, 1, 191), 'grass_side'],
    [Vertex.of( 0.5, -0.5, -0.5, 1, 0, 191), Vertex.of(-0.5,  0.5, -0.5, 0, 1, 191), Vertex.of( 0.5,  0.5, -0.5, 1, 1, 191), 'grass_side'],
    [Vertex.of(-0.5, -0.5,  0.5, 1, 0, 191), Vertex.of( 0.5, -0.5,  0.5, 0, 0, 191), Vertex.of( 0.5,  0.5,  0.5, 0, 1, 191), 'grass_side'],
    [Vertex.of(-0.5, -0.5,  0.5, 1, 0, 191), Vertex.of( 0.5,  0.5,  0.5, 0, 1, 191), Vertex.of(-0.5,  0.5,  0.5, 1, 1, 191), 'grass_side'],
    [Vertex.of(-0.5, -0.5, -0.5, 1, 0, 159), Vertex.of(-0.5, -0.5,  0.5, 0, 0, 159), Vertex.of(-0.5,  0.5,  0.5, 0, 1, 159), 'grass_side'],
    [Vertex.of(-0.5, -0.5, -0.5, 1, 0, 159), Vertex.of(-0.5,  0.5,  0.5, 0, 1, 159), Vertex.of(-0.5,  0.5, -0.5, 1, 1, 159), 'grass_side'],
    [Vertex.of( 0.5, -0.5,  0.5, 1, 0, 159), Vertex.of( 0.5, -0.5, -0.5, 0, 0, 159), Vertex.of( 0.5,  0.5, -0.5, 0, 1, 159), 'grass_side'],
    [Vertex.of( 0.5, -0.5,  0.5, 1, 0, 159), Vertex.of( 0.5,  0.5, -0.5, 0, 1, 159), Vertex.of( 0.5,  0.5,  0.5, 1, 1, 159), 'grass_side'],
    [Vertex.of(-0.5,  0.5,  0.5, 1, 0, 255), Vertex.of( 0.5,  0.5,  0.5, 0, 0, 255), Vertex.of( 0.5,  0.5, -0.5, 0, 1, 255), 'grass_top'],
    [Vertex.of(-0.5,  0.5,  0.5, 1, 0, 255), Vertex.of( 0.5,  0.5, -0.5, 0, 1, 255), Vertex.of(-0.5,  0.5, -0.5, 1, 1, 255), 'grass_top'],
    [Vertex.of( 0.5, -0.5,  0.5, 0, 1, 95), Vertex.of(-0.5, -0.5, -0.5, 1, 0, 95), Vertex.of( 0.5, -0.5, -0.5, 0, 0, 95), 'dirt'],
    [Vertex.of( 0.5, -0.5,  0.5, 0, 1, 95), Vertex.of(-0.5, -0.5,  0.5, 1, 1, 95), Vertex.of(-0.5, -0.5, -0.5, 1, 0, 95), 'dirt'],
)

let tris = [
    // Front
    [[[ 0.5, -0.5, -0.5, [191, 0, 0], [1, 0], 191], [-0.5, -0.5, -0.5, [0, 191, 0], [0, 0], 191], [-0.5,  0.5, -0.5, [0, 0, 191], [0, 1], 191]], 'grass_side'],
    [[[ 0.5, -0.5, -0.5, [191, 0, 0], [1, 0], 191], [-0.5,  0.5, -0.5, [0, 191, 0], [0, 1], 191], [ 0.5,  0.5, -0.5, [0, 0, 191], [1, 1], 191]], 'grass_side'],
    // Back
    [[[-0.5, -0.5,  0.5, [191, 0, 0], [1, 0], 191], [ 0.5, -0.5,  0.5, [0, 191, 0], [0, 0], 191], [ 0.5,  0.5,  0.5, [0, 0, 191], [0, 1], 191]], 'grass_side'],
    [[[-0.5, -0.5,  0.5, [191, 0, 0], [1, 0], 191], [ 0.5,  0.5,  0.5, [0, 191, 0], [0, 1], 191], [-0.5,  0.5,  0.5, [0, 0, 191], [1, 1], 191]], 'grass_side'],
    // Left
    [[[-0.5, -0.5, -0.5, [159, 0, 0], [1, 0], 159], [-0.5, -0.5,  0.5, [0, 159, 0], [0, 0], 159], [-0.5,  0.5,  0.5, [0, 0, 159], [0, 1], 159]], 'grass_side'],
    [[[-0.5, -0.5, -0.5, [159, 0, 0], [1, 0], 159], [-0.5,  0.5,  0.5, [0, 159, 0], [0, 1], 159], [-0.5,  0.5, -0.5, [0, 0, 159], [1, 1], 159]], 'grass_side'],
    // Right
    [[[ 0.5, -0.5,  0.5, [159, 0, 0], [1, 0], 159], [ 0.5, -0.5, -0.5, [0, 159, 0], [0, 0], 159], [ 0.5,  0.5, -0.5, [0, 0, 159], [0, 1], 159]], 'grass_side'],
    [[[ 0.5, -0.5,  0.5, [159, 0, 0], [1, 0], 159], [ 0.5,  0.5, -0.5, [0, 159, 0], [0, 1], 159], [ 0.5,  0.5,  0.5, [0, 0, 159], [1, 1], 159]], 'grass_side'],
    // Top
    [[[-0.5,  0.5,  0.5, [255, 0, 0], [1, 0], 255], [ 0.5,  0.5,  0.5, [0, 255, 0], [0, 0], 255], [ 0.5,  0.5, -0.5, [0, 0, 255], [0, 1], 255]], 'grass_top'],
    [[[-0.5,  0.5,  0.5, [255, 0, 0], [1, 0], 255], [ 0.5,  0.5, -0.5, [0, 255, 0], [0, 1], 255], [-0.5,  0.5, -0.5, [0, 0, 255], [1, 1], 255]], 'grass_top'],
    // Bottom
    [[[ 0.5, -0.5,  0.5, [255, 0, 0], [0, 1], 95], [-0.5, -0.5, -0.5, [0, 255, 0], [1, 0], 95], [ 0.5, -0.5, -0.5, [0, 0, 255], [0, 0], 95]], 'dirt'],
    [[[ 0.5, -0.5,  0.5, [255, 0, 0], [0, 1], 95], [-0.5, -0.5,  0.5, [0, 255, 0], [1, 1], 95], [-0.5, -0.5, -0.5, [0, 0, 255], [1, 0], 95]], 'dirt'],
]

let texture_contexts = {}

for (let tri of tris) {
    let texture_name = tri[1]

    if (!texture_contexts[texture_name]) {
        let image = new Image()

        const image_canvas = document.createElement('canvas')
        const image_context = image_canvas.getContext('2d')

        image.onload = () => {
            image_context.canvas.width = image.width
            image_context.canvas.height = image.height

            image_context.drawImage(image, 0, 0)

            texture_contexts[texture_name] = image_context
        }

        image.src = `js/${texture_name}.png`
    }
}

let camera_vertex = [[0], [0], [-4]]
let angle_vertex = [[0.0], [0.0], [0]]
let field_of_view_degrees = 70

function draw(camera_vertex, angle_vertex, field_of_view_degrees) {
    let projections = []

    for (let tri of tris) {
        let projected_tri = []

        let render = true

        for (let vertex of tri[0]) {
            let project_vertex = [[vertex[0]], [vertex[1]], [vertex[2]]]

            let values = project_matrix(project_vertex, camera_vertex, angle_vertex, field_of_view_degrees, screen_width, screen_height)

            if (Array.isArray(values)) {
                projected_tri.push([values[0], values[1], values[2], vertex[3], vertex[4], vertex[5]])
            } else {
                render = false
            }
        }

        projected_tri.push(tri[1])

        if (render) {
            projections.push(projected_tri)
        }
    }

    let image_data = []

    for (let y = 0; y < screen_height; y++) {
        for (let x = 0; x < screen_width; x++) {
            let red = 0
            let green = 0
            let blue = 0

            for (let tri of projections) {
                let w0 = edge(tri[1][0], tri[1][1], tri[2][0], tri[2][1], x, y)
                let w1 = edge(tri[2][0], tri[2][1], tri[0][0], tri[0][1], x, y)
                let w2 = edge(tri[0][0], tri[0][1], tri[1][0], tri[1][1], x, y)

                if (w0 <= 0 && w1 <= 0 && w2 <= 0) {
                    let area = edge(tri[0][0], tri[0][1], tri[1][0], tri[1][1], tri[2][0], tri[2][1])

                    w0 /= area
                    w1 /= area
                    w2 /= area

                    let z = w0 / tri[0][2] + w1 / tri[1][2] + w2 / tri[2][2]

                    let x = (w0 * tri[0][4][0] / tri[0][2] + w1 * tri[1][4][0] / tri[1][2] + w2 * tri[2][4][0] / tri[2][2]) / z
                    let y = (w0 * tri[0][4][1] / tri[0][2] + w1 * tri[1][4][1] / tri[1][2] + w2 * tri[2][4][1] / tri[2][2]) / z

                    let b = (w0 * tri[0][5] / tri[0][2] + w1 * tri[1][5] / tri[1][2] + w2 * tri[2][5] / tri[2][2]) / z

                    if (!isNaN(x) && !isNaN(y) && texture_contexts[tri[3]]) {
                        let image_context = texture_contexts[tri[3]]
                        let texture_width = image_context.canvas.width
                        let texture_height = image_context.canvas.height
                        let color_data = image_context.getImageData(Math.min(texture_width - 1, Math.floor(x * texture_width)), Math.min(texture_height - 1, Math.floor(texture_height - y * texture_height)), 1, 1)

                        red = color_data.data[0] * (b / 255)
                        green = color_data.data[1] * (b / 255)
                        blue = color_data.data[2] * (b / 255)
                    }
                }
            }

            image_data.push(red)
            image_data.push(green)
            image_data.push(blue)
            image_data.push(255)
        }
    }

    let prev_image_data = context.createImageData(screen_width, screen_height)

    for (let i = 0; i < image_data.length; i++) {
        prev_image_data.data[i] = image_data[i]
    }

    let new_canvas = document.createElement('canvas')
    let new_context = new_canvas.getContext('2d')

    new_context.canvas.width = screen_width
    new_context.canvas.height = screen_height

    new_context.putImageData(prev_image_data, 0, 0)

    context.drawImage(new_context.canvas, 0, 0, screen_width * scale, screen_height * scale)
}

let keys = {}

let speed = 0.1

document.onkeydown = (event) => {
    keys[event.code] = 1
}

document.onkeyup = event => {
    delete keys[event.code]
}

function rotate(theta, pos) {
    return multiply_matrix(
        [[Math.cos(theta), -1 * Math.sin(theta)], [Math.sin(theta), Math.cos(theta)]],
        pos
    )
}

let x = 0
let y = 0

document.onmousemove = (event) => {
    if (document.pointerLockElement === canvas) {
        x += event.movementX * 2.0
        y += event.movementY * 2.0
        if (y / canvas.height < -0.5 * Math.PI) {
            y = -0.5 * Math.PI * canvas.height
        }
        if (y / canvas.height > 0.5 * Math.PI) {
            y = 0.5 * Math.PI * canvas.height
        }
        angle_vertex[0][0] = y / canvas.height
        angle_vertex[1][0] = x / canvas.width
    }
}

canvas.onclick = () => canvas.requestPointerLock()

const update = () => {
    let m = [[0.0], [0.0]]
    if ('KeyW' in keys) {
        m[1][0] += speed
    }
    if ('KeyS' in keys) {
        m[1][0] -= speed
    }
    if ('KeyA' in keys) {
        m[0][0] += speed
    }
    if ('KeyD' in keys) {
        m[0][0] -= speed
    }
    if ('Space' in keys) {
        camera_vertex[1][0] += speed
    }
    if ('ShiftLeft' in keys) {
        camera_vertex[1][0] -= speed
    }
    let pos = rotate(angle_vertex[1][0], m)
    camera_vertex[0][0] += -1 * pos[0][0]
    camera_vertex[2][0] += pos[1][0]

    draw(camera_vertex, angle_vertex, field_of_view_degrees)

    requestAnimationFrame(update)
}

update()

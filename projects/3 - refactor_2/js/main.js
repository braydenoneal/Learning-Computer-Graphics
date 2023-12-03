const canvas = document.createElement('canvas')
document.body.appendChild(canvas)
const context = canvas.getContext('2d')

context.canvas.width = window.innerWidth
context.canvas.height = window.innerHeight

let scale = 6

let screen_width = Math.floor(canvas.width / scale)
let screen_height = Math.floor(canvas.height / scale)

context.imageSmoothingEnabled = false

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

let points = []

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

        points.push([x, y])

        return [Math.floor(x), Math.floor(y), d[2][0]]
    } else {
        return NaN
    }
}

function get_line_values(x1, y1, x2, y2, d1, d2, c1, c2) {
    let values = []

    let y_distance = Math.abs(y2 - y1)
    let y_direction = y2 > y1 ? 1 : -1

    let x_distance = x2 - x1
    let x_step = x_distance / y_distance

    let d_distance = d2 - d1
    let d_step = d_distance / y_distance

    let red_distance = c2[0] - c1[0]
    let red_step = red_distance / y_distance

    let green_distance = c2[1] - c1[1]
    let green_step = green_distance / y_distance

    let blue_distance = c2[2] - c1[2]
    let blue_step = blue_distance / y_distance

    for (let i = 0; i < y_distance; i++) {
        let y = y1 + i * y_direction;
        let x = Math.round(x1 + i * x_step)
        let d = d1 + i * d_step
        let red = c1[0] + i * red_step
        let green = c1[1] + i * green_step
        let blue = c1[2] + i * blue_step

        values.push([x, y, d, [red, green, blue]])
    }

    return values
}

function get_tri_values(line_values) {
    let values = []

    let y_pairs = []

    for (let line_value of line_values) {
        let add = true

        values.push(line_value)

        for (let i = 0; i < y_pairs.length; i++) {
            if (line_value[1] === y_pairs[i][1] && line_value[0] !== y_pairs[i][0] && y_pairs[i].length === 4) {
                if (line_value[1] > y_pairs[i][1]) {
                    y_pairs[i] = [...line_value, ...y_pairs[i]]
                } else {
                    y_pairs[i] = [...y_pairs[i], ...line_value]
                }
                add = false
            }
        }

        if (add) {
            y_pairs.push(line_value)
        }
    }

    for (let y_pair of y_pairs) {
        if (y_pair.length > 4) {
            let x1 = y_pair[0]
            let x2 = y_pair[4]
            let y = y_pair[1]
            let d1 = y_pair[2]
            let d2 = y_pair[6]
            let c1 = y_pair[3]
            let c2 = y_pair[7]

            let x_distance = Math.abs(x2 - x1)
            let x_direction = x2 > x1 ? 1 : -1

            let d_distance = d2 - d1
            let d_step = d_distance / x_distance

            let red_distance = c2[0] - c1[0]
            let red_step = red_distance / x_distance

            let green_distance = c2[1] - c1[1]
            let green_step = green_distance / x_distance

            let blue_distance = c2[2] - c1[2]
            let blue_step = blue_distance / x_distance

            let d_direction = d2 < d1 ? 0.01 : -0.01

            for (let i = 1; i < x_distance; i++) {
                let x = x1 + i * x_direction
                let d = d1 + i * d_step
                let red = c1[0] + i * red_step
                let green = c1[1] + i * green_step
                let blue = c1[2] + i * blue_step

                values.push([x, y, d + d_direction, [red, green, blue]])
            }
        }
    }

    return values
}

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
    [[0.0, 0.0, 0.0, [255, 0, 0]], [0.0, 0.0, 1.0, [0, 255, 0]], [0.0, 1.0, 0.0, [0, 0, 255]]],

    // [[0.5, -1.0, 2.0, [255, 0, 0]], [0.5, 2.0, -1.0, [0, 255, 0]], [0.5, 2.0, 2.0, [0, 0, 255]]],
    // [[0.5, -1.0, -1.0, [255, 0, 0]], [0.5, -1.0, 2.0, [0, 255, 0]], [0.5, 2.0, -1.0, [0, 0, 255]]],

    // [[0.0, 1.0, 1.0, [127, 127, 127]], [1.0, 0.0, 1.0, [63, 63, 63]], [1.0, 1.0, 1.0, [127, 127, 127]]],
    // [[0.0, 0.0, 1.0, [63, 63, 63]], [0.0, 1.0, 1.0, [127, 127, 127]], [1.0, 0.0, 1.0, [63, 63, 63]]],
]

function draw(camera_vertex, angle_vertex, field_of_view_degrees) {
    let projections = []

    for (let tri of tris) {
        let projected_tri = []

        let render = true

        for (let vertex of tri) {
            let project_vertex = [[vertex[0]], [vertex[1]], [vertex[2]]]

            let values = project_matrix(project_vertex, camera_vertex, angle_vertex, field_of_view_degrees, screen_width, screen_height)

            if (Array.isArray(values)) {
                projected_tri.push([values[0], values[1], values[2], vertex[3]])
            } else {
                render = false
            }
        }

        if (render) {
            projections.push(projected_tri)
        }
    }

    let data = Array.from({ length: screen_width }, () => Array.from({ length: screen_height }, () => false))

    for (let projection of projections) {
        let line_values = []

        let x1 = projection[0][0]
        let y1 = projection[0][1]
        let x2 = projection[1][0]
        let y2 = projection[1][1]
        let x3 = projection[2][0]
        let y3 = projection[2][1]

        let d1 = projection[0][2]
        let d2 = projection[1][2]
        let d3 = projection[2][2]

        let c1 = projection[0][3]
        let c2 = projection[1][3]
        let c3 = projection[2][3]

        let line_values_1 = get_line_values(x1, y1, x2, y2, d1, d2, c1, c2)
        let line_values_2 = get_line_values(x2, y2, x3, y3, d2, d3, c2, c3)
        let line_values_3 = get_line_values(x3, y3, x1, y1, d3, d1, c3, c1)

        for (let line_value of line_values_1) {
            line_values.push(line_value)
        }

        for (let line_value of line_values_2) {
            line_values.push(line_value)
        }

        for (let line_value of line_values_3) {
            line_values.push(line_value)
        }

        let tri_values = get_tri_values(line_values)

        for (let tri_value of tri_values) {
            let x = tri_value[0]
            let y = tri_value[1]
            let depth = tri_value[2]
            let color = tri_value[3]

            if (x >= 0 && y >= 0 && x < screen_width && y < screen_height) {
                if (data[x][y]) {
                    if (data[x][y][0] > depth) {
                        data[x][y] = [depth, color]
                    } else if (data[x][y][0] === depth) {
                        data[x][y] = [depth, color]
                    }
                } else {
                    data[x][y] = [depth, color]
                }
            }
        }
    }

    let image_data = []

    for (let y = 0; y < screen_height; y++) {
        for (let x = 0; x < screen_width; x++) {
            if (data[x][y]) {
                let color = data[x][y][1]
                image_data.push(Math.round(color[0]))
                image_data.push(Math.round(color[1]))
                image_data.push(Math.round(color[2]))
            } else {
                image_data.push(0)
                image_data.push(0)
                image_data.push(0)
            }
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

    new_context.fillStyle = 'rgba(255, 255, 255, 255)'

    // for (let point of points) {
    //     new_context.fillRect(Math.round(point[0]), Math.round(point[1]), 1, 1)
    // }

    context.drawImage(new_context.canvas, 0, 0, screen_width * scale, screen_height * scale)
}

let camera_vertex = [[3.0], [2.5], [-5.1]]
let angle_vertex = [[0.35], [-0.4], [0]]
let field_of_view_degrees = 70

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
            y = 0.5 * Math.PI * canvas.height
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

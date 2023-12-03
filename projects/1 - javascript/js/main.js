const canvas = document.createElement('canvas')
document.body.appendChild(canvas)
const context = canvas.getContext('2d')

context.canvas.width = window.innerWidth
context.canvas.height = window.innerHeight

let cos = Math.cos
let sin = Math.sin

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

function project(a, c, t, e) {
    let first = [
        [1, 0, 0],
        [0, cos(t[0][0]), sin(t[0][0])],
        [0, -1 * sin(t[0][0]), cos(t[0][0])]
    ]
    let second = [
        [cos(t[1][0]), 0, -1 * sin(t[1][0])],
        [0, 1, 0],
        [sin(t[1][0]), 0, cos(t[1][0])]
    ]
    let third = [
        [cos(t[2][0]), sin(t[2][0]), 0],
        [-1 * sin(t[2][0]), cos(t[2][0]), 0],
        [0, 0, 1]
    ]
    let fourth = subtract(a, c)

    let d = multiply(multiply(first, second), multiply(third, fourth))

    if (d[2] <= 0) {
        d[2] = NaN
    }

    let x = e[2][0] / d[2][0] * d[0][0] + e[0][0]
    let y = e[2][0] / d[2][0] * d[1][0] + e[1][0]

    return [x, y, d[2][0]]
}

import json from "./teapot.json" assert { type: 'json' }

let tris = json.tris

for (let tri of tris) {
    for(let vertex of tri) {
        vertex[1][0] *= -1
        vertex[1][0] *= 0.25
        vertex[2][0] *= 0.25
        vertex[0][0] *= 0.25
    }
}

let tris2 = [
    [[[0.0], [1.0], [1.0]], [[1.0], [0.0], [1.0]], [[1.0], [1.0], [1.0]]],
    [[[0.0], [0.0], [1.0]], [[0.0], [1.0], [1.0]], [[1.0], [0.0], [1.0]]],
    [[[0.0], [1.0], [1.0]], [[1.0], [1.0], [0.0]], [[1.0], [1.0], [1.0]]],
    [[[0.0], [1.0], [0.0]], [[0.0], [1.0], [1.0]], [[1.0], [1.0], [0.0]]],
    [[[1.0], [0.0], [1.0]], [[1.0], [1.0], [0.0]], [[1.0], [1.0], [1.0]]],
    [[[1.0], [0.0], [0.0]], [[1.0], [0.0], [1.0]], [[1.0], [1.0], [0.0]]],
    [[[0.0], [1.0], [0.0]], [[1.0], [0.0], [0.0]], [[1.0], [1.0], [0.0]]],
    [[[0.0], [0.0], [0.0]], [[0.0], [1.0], [0.0]], [[1.0], [0.0], [0.0]]],
    [[[0.0], [0.0], [1.0]], [[1.0], [0.0], [0.0]], [[1.0], [0.0], [1.0]]],
    [[[0.0], [0.0], [0.0]], [[0.0], [0.0], [1.0]], [[1.0], [0.0], [0.0]]],
    [[[0.0], [0.0], [1.0]], [[0.0], [1.0], [0.0]], [[0.0], [1.0], [1.0]]],
    [[[0.0], [0.0], [0.0]], [[0.0], [0.0], [1.0]], [[0.0], [1.0], [0.0]]]
]

for (let tri of tris2) {
    for(let vertex of tri) {
        vertex[0][0] -= 0.5
        // vertex[1][0] += 0.25
        vertex[2][0] -= 0.5
        vertex[0][0] *= 2
        vertex[1][0] *= 2
        vertex[2][0] *= 2
    }
}

function draw(c, t) {
    let e = [[0.0], [0.0], [1.0]]

    let projected_tris2 = []

    for (let tri of tris2) {
        let projected_tri = []

        for (let vertex of tri) {
            projected_tri.push(project(vertex, c, t, e))
        }

        projected_tris2.push(projected_tri)
    }

    projected_tris2.sort((a, b) => {
        let a_sum = a[0][2] + a[1][2] + a[2][2]
        let b_sum = b[0][2] + b[1][2] + b[2][2]

        return b_sum - a_sum
    })

    for (let index = 0; index < projected_tris2.length; index++) {
        let projected_tri = projected_tris2[index]

        let first_x = canvas.width / 2 + (projected_tri[0][0] * canvas.height / 2)
        let second_x = canvas.width / 2 + (projected_tri[1][0] * canvas.height / 2)
        let third_x = canvas.width / 2 + (projected_tri[2][0] * canvas.height / 2)
        let first_y = canvas.height / 2 + (projected_tri[0][1] * canvas.height / 2)
        let second_y = canvas.height / 2 + (projected_tri[1][1] * canvas.height / 2)
        let third_y = canvas.height / 2 + (projected_tri[2][1] * canvas.height / 2)

        context.beginPath()
        context.moveTo(first_x, first_y)
        context.lineTo(second_x, second_y)
        context.lineTo(third_x, third_y)
        context.lineTo(first_x, first_y)
        context.fillStyle = `rgba(0, ${index / projected_tris2.length * 256}, 0, 255)`
        context.fill();
    }

    let projected_tris = []

    for (let tri of tris) {
        let projected_tri = []

        for (let vertex of tri) {
            projected_tri.push(project(vertex, c, t, e))
        }

        projected_tris.push(projected_tri)
    }

    projected_tris.sort((a, b) => {
        let a_sum = a[0][2] + a[1][2] + a[2][2]
        let b_sum = b[0][2] + b[1][2] + b[2][2]

        return b_sum - a_sum
    })

    for (let index = 0; index < projected_tris.length; index++) {
        let projected_tri = projected_tris[index]

        let first_x = canvas.width / 2 + (projected_tri[0][0] * canvas.height / 2)
        let second_x = canvas.width / 2 + (projected_tri[1][0] * canvas.height / 2)
        let third_x = canvas.width / 2 + (projected_tri[2][0] * canvas.height / 2)
        let first_y = canvas.height / 2 + (projected_tri[0][1] * canvas.height / 2)
        let second_y = canvas.height / 2 + (projected_tri[1][1] * canvas.height / 2)
        let third_y = canvas.height / 2 + (projected_tri[2][1] * canvas.height / 2)

        context.beginPath()
        context.moveTo(first_x, first_y)
        context.lineTo(second_x, second_y)
        context.lineTo(third_x, third_y)
        context.lineTo(first_x, first_y)
        context.fillStyle = `rgba(${index / projected_tris.length * 256}, 0, 0, 255)`
        context.fill();
    }
}

let x = 0
let y = 0

let c = [[0], [0], [0]]
let t = [[0], [0], [0]]

document.onmousemove = (event) => {
    if (document.pointerLockElement === canvas) {
        x += event.movementX * 2.0
        y -= event.movementY * 2.0
        if (y / canvas.height < -0.5 * Math.PI) {
            y = -0.5 * Math.PI * canvas.height
        }
        if (y / canvas.height > 0.5 * Math.PI) {
            y = 0.5 * Math.PI * canvas.height
        }
        t[0] = [y / canvas.height]
        t[1] = [x / canvas.width]
    }
}

let keys = {}

let speed = 0.1

document.onkeydown = (event) => {
    keys[event.code] = 1
    render(c, t)
}

document.onkeyup = event => {
    delete keys[event.code]
    render(c, t)
}

function rotate(theta, pos) {
    return multiply(
        [[cos(theta), -1 * sin(theta)], [sin(theta), cos(theta)]],
        pos
    )
}

function render(c, t) {
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
        c[1][0] -= speed
    }
    if ('ShiftLeft' in keys) {
        c[1][0] += speed
    }
    let pos = rotate(t[1][0], m)
    c[0][0] += -1 * pos[0][0]
    c[2][0] += pos[1][0]
    context.fillStyle = `rgba(0, 0, 0, 255)`
    context.fillRect(0, 0, canvas.width, canvas.height)
    draw(c, t)
}

canvas.onclick = () => canvas.requestPointerLock()

let update = () => {
    render(c, t)
    requestAnimationFrame(update)
}

update()

/*
Better depth buffer:

Plot each point as a pixel paired with its depth(z value after transform)
Interpolate as if drawing pixels, but interpolate the depth between vertices
Draw all pixels on screen from largest to smallest depth value
 */

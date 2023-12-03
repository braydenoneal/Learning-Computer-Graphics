const canvas = document.createElement('canvas')
document.body.appendChild(canvas)
const context = canvas.getContext('2d')

context.canvas.width = window.innerWidth
context.canvas.height = window.innerHeight

let scale = 8

function draw(x, y, color) {
    context.fillStyle = `rgba(${color}, ${color}, ${color}, 255)`
    context.fillRect(x * scale, y * scale, scale, scale)
}

let tris = [
    [
        [1, 1],
        [64, 48],
        [84, 8]
    ],
    [
        [123, 92],
        [64, 48],
        [84, 8]
    ]
]

function verts_to_line_pos(x1, y1, x2, y2) {
    let points = []

    let y_distance = Math.abs(y2 - y1)
    let y_direction = y2 > y1 ? 1 : -1

    let x_distance = x2 - x1
    let x_step = x_distance / y_distance

    for (let i = 0; i < y_distance; i++) {
        let y = y1 + i * y_direction
        let x = x1 + Math.round(i * x_step)

        points.push([x, y])
    }

    return points
}

for (let tri of tris) {
    let x1 = tri[0][0]
    let y1 = tri[0][1]
    let x2 = tri[1][0]
    let y2 = tri[1][1]
    let x3 = tri[2][0]
    let y3 = tri[2][1]

    let points_1 = verts_to_line_pos(x1, y1, x2, y2)
    let points_2 = verts_to_line_pos(x2, y2, x3, y3)
    let points_3 = verts_to_line_pos(x3, y3, x1, y1)

    let points = []

    for (let point of points_1) {
        draw(point[0], point[1], '255')
        points.push(point)
    }
    for (let point of points_2) {
        draw(point[0], point[1], '255')
        points.push(point)
    }
    for (let point of points_3) {
        draw(point[0], point[1], '255')
        points.push(point)
    }

    let pairs = []

    for (let point of points) {
        let add = true
        for (let pair of pairs) {
            if (point[1] === pair[1] && pair.length === 2 && point[0] !== pair[0]) {
                pair.push(point[0])
                add = false
            }
        }

        if (add) {
            pairs.push(point)
        }
    }

    for (let pair of pairs) {
        let y = pair[1]
        let x1 = pair[0]
        let x2 = pair[2]

        let x_direction = x2 > x1 ? 1 : -1

        for (let i = 1; i < Math.abs(x2 - x1); i++) {
            draw(x1 + i * x_direction, y, 127)
        }
    }
}

for (let tri of tris) {
    for (let vert of tri) {
        context.fillStyle = `rgba(255, 0, 0, 255)`
        context.fillRect(vert[0] * scale, vert[1] * scale, scale / 2, scale / 2)
    }
}

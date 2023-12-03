export class Vector {
    constructor(x, y, z) {
        this.x = x
        this.y = y
        this.z = z
    }

    subtract(vector) {
        return new Vector(this.x - vector.x, this.y - vector.y, this.z - vector.z)
    }

    multiply_value(vector) {
        return this.x * vector.x + this.y * vector.y + this.z * vector.z
    }

    to_matrix() {
        return new Matrix([[this.x], [this.y], [this.z]])
    }
}

export class Matrix {
    constructor(matrix) {
        this.matrix = matrix
    }

    subtract(matrix) {
        let a = this.matrix
        let b = matrix.matrix

        let c = []

        for (let row = 0; row < a.length; row++) {
            let c_row = []

            for (let column = 0; column < a[0].length; column++) {
                c_row.push(a[row][column] - b[row][column])
            }

            c.push(c_row)
        }

        return new Matrix(c)
    }

    multiply(matrix) {
        let a = this.matrix
        let b = matrix.matrix

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

        return new Matrix(c)
    }

    get(row, column) {
        return this.matrix[row][column]
    }

    get_row(row) {
        return this.matrix[row][0]
    }
}

export class Vertex {
    constructor(x, y, z, u, v, b) {
        this.vector = new Vector(x, y, z)
        this.x = this.vector.x
        this.y = this.vector.y
        this.z = this.vector.z
        this.u = u
        this.v = v
        this.b = b
    }

    to_screen_space(camera_vector, angle_vector) {
        let x_rotation = new Matrix([
            [1, 0, 0],
            [0, Math.cos(angle_vector.x), Math.sin(angle_vector.x)],
            [0, -1 * Math.sin(angle_vector.x), Math.cos(angle_vector.x)]
        ])

        let y_rotation = new Matrix([
            [Math.cos(angle_vector.y), 0, -1 * Math.sin(angle_vector.y)],
            [0, 1, 0],
            [Math.sin(angle_vector.y), 0, Math.cos(angle_vector.y)]
        ])

        let z_rotation = new Matrix([
            [Math.cos(angle_vector.z), Math.sin(angle_vector.z), 0],
            [-1 * Math.sin(angle_vector.z), Math.cos(angle_vector.z), 0],
            [0, 0, 1]
        ])

        let camera_shift = this.vector.subtract(camera_vector).to_matrix()

        let d = x_rotation.multiply(y_rotation).multiply(z_rotation).multiply(camera_shift)

        return new Vertex(d.get_row(0), d.get_row(1), d.get_row(2), this.u, this.v, this.b)
    }

    perspective_project(d, field_of_view_degrees, screen_width, screen_height) {
        let field_of_view_radians = field_of_view_degrees * Math.PI / 180

        let display_surface_distance = 1 / (Math.tan(field_of_view_radians / 2))

        let x = (display_surface_distance * d.get_row(0) / d.get_row(2)) * screen_height / 2 + screen_width / 2
        let y = (display_surface_distance * d.get_row(1) / d.get_row(2)) * -1 * screen_height / 2 + screen_height / 2

        return new ProjectedVertex(new XY(x, y), d.get_row(2), this)
    }

    static of(...parameters) {
        return new Vertex(...parameters)
    }
}

export class ProjectedVertex {
    constructor(xy, depth, vertex) {
        this.xy = xy
        this.x = xy.x
        this.y = xy.y
        this.z = depth
        this.u = vertex.u
        this.v = vertex.v
        this.b = vertex.b
    }
}

export class XY {
    constructor(x, y) {
        this.x = x
        this.y = y
    }

    static edge(p, v0, v1) {
        return (v1.x - p.x) * (v0.y - p.y) - (v1.y - p.y) * (v0.x - p.x)
    }
}

export class Tri {
    constructor(v0, v1, v2, texture) {
        this.v0 = v0
        this.v1 = v1
        this.v2 = v2
        this.texture = texture
    }

    static clip_tris(n, d, tris) {
        let clipped_tris = []

        for (let tri of tris) {
            clipped_tris = [...clipped_tris, ...tri.clip(n, d)]
        }

        return clipped_tris
    }

    clip(n, d) {
        let d0 = n.multiply_value(this.v0.vector) + d
        let d1 = n.multiply_value(this.v1.vector) + d
        let d2 = n.multiply_value(this.v2.vector) + d

        let inside = []
        let outside = []

        if (d0 < 0) {
            outside.push([this.v0, 0])
        } else {
            inside.push([this.v0, 0])
        }
        if (d1 < 0) {
            outside.push([this.v1, 1])
        } else {
            inside.push([this.v1, 1])
        }
        if (d2 < 0) {
            outside.push([this.v2, 2])
        } else {
            inside.push([this.v2, 2])
        }

        if (outside.length === 3) {
            return []
        } else if (outside.length === 2) {
            let order = (inside[0][1] === 1) !== (outside[0][1] < outside[1][1])

            let a = inside[0][0]
            let b = outside[order ? 0 : 1][0]
            let c = outside[!order ? 0 : 1][0]

            let t = (-d - n.multiply_value(a.vector)) / n.multiply_value(b.vector.subtract(a.vector))

            let b_x = a.x + t * (b.x - a.x)
            let b_y = a.y + t * (b.y - a.y)
            let b_z = a.z + t * (b.z - a.z)
            let b_u = a.u + t * (b.u - a.u)
            let b_v = a.v + t * (b.v - a.v)
            let b_b = a.b + t * (b.b - a.b)

            t = (-d - n.multiply_value(a.vector)) / n.multiply_value(c.vector.subtract(a.vector))

            let c_x = a.x + t * (c.x - a.x)
            let c_y = a.y + t * (c.y - a.y)
            let c_z = a.z + t * (c.z - a.z)
            let c_u = a.u + t * (c.u - a.u)
            let c_v = a.v + t * (c.v - a.v)
            let c_b = a.b + t * (c.b - a.b)

            let b_prime = new Vertex(b_x, b_y, b_z, b_u, b_v, b_b)
            let c_prime = new Vertex(c_x, c_y, c_z, c_u, c_v, c_b)

            return [new Tri(a, b_prime, c_prime, this.texture)]
        } else if (outside.length === 1) {
            let order = (outside[0][1] === 1) !== (inside[0][1] < inside[1][1])

            let a = inside[order ? 0 : 1][0]
            let b = inside[!order ? 0 : 1][0]
            let c = outside[0][0]

            let t = (-d - n.multiply_value(a.vector)) / n.multiply_value(c.vector.subtract(a.vector))

            let a_x = a.x + t * (c.x - a.x)
            let a_y = a.y + t * (c.y - a.y)
            let a_z = a.z + t * (c.z - a.z)
            let a_u = a.u + t * (c.u - a.u)
            let a_v = a.v + t * (c.v - a.v)
            let a_b = a.b + t * (c.b - a.b)

            t = (-d - n.multiply_value(b.vector)) / n.multiply_value(c.vector.subtract(b.vector))

            let b_x = b.x + t * (c.x - b.x)
            let b_y = b.y + t * (c.y - b.y)
            let b_z = b.z + t * (c.z - b.z)
            let b_u = b.u + t * (c.u - b.u)
            let b_v = b.v + t * (c.v - b.v)
            let b_b = b.b + t * (c.b - b.b)

            let a_prime = new Vertex(a_x, a_y, a_z, a_u, a_v, a_b)
            let b_prime = new Vertex(b_x, b_y, b_z, b_u, b_v, b_b)

            let tri0 = new Tri(a, b, a_prime, this.texture)
            let tri1 = new Tri(a_prime, b, b_prime, this.texture)

            return [tri0, tri1]
        } else {
            return [this]
        }
    }

    static of(tris_data) {
        let tris = []

        for (let tri of tris_data) {
            tris.push(new Tri(...tri))
        }

        return tris
    }
}

export class ProjectedTri {
    constructor(v0, v1, v2, tri) {
        this.v0 = v0
        this.v1 = v1
        this.v2 = v2
        this.texture = tri.texture
        this.area = XY.edge(this.v0, this.v1, this.v2)
    }

    get_weights(xy) {
        let w0 = XY.edge(this.v1.xy, this.v2.xy, xy)
        let w1 = XY.edge(this.v2.xy, this.v0.xy, xy)
        let w2 = this.area - w0 - w1

        return new BarycentricCoordinate(w0, w1, w2)
    }

    get_scaled_weights(weights) {
        return new BarycentricCoordinate(weights.w0 / this.area, weights.w1 / this.area, weights.w2 / this.area)
    }

    is_inside(weights) {
        return this.area !== 0 && weights.w0 >= 0 && weights.w1 >= 0 && weights.w2 >= 0
    }

    get_depth_scale(weights) {
        return weights.w0 / this.v0.z + weights.w1 / this.v1.z + weights.w2 / this.v2.z
    }

    get_value(v0, v1, v2, weights, depth_scale) {
        return (weights.w0 * v0 / this.v0.z + weights.w1 * v1 / this.v1.z + weights.w2 * v2 / this.v2.z) / depth_scale
    }
}

export class BarycentricCoordinate {
    constructor(w0, w1, w2) {
        this.w0 = w0
        this.w1 = w1
        this.w2 = w2
    }
}

export class FrameBuffer {
    constructor(width, height) {
        this.pixels = Array.from({ length: height }, () => Array.from({ length: width }, () => new Color(0, 0, 0, 255, Infinity)))
    }

    set(x, y, color) {
        this.pixels[y][x] = color
    }

    get(x, y) {
        return this.pixels[y][x]
    }

    to_image_data() {
        let image_data = []

        for (let row of this.pixels) {
            for (let color of row) {
                image_data.push(color.r)
                image_data.push(color.g)
                image_data.push(color.b)
                image_data.push(color.a)
            }
        }

        return image_data
    }
}

export class Color {
    constructor(r, g, b, a, depth) {
        this.r = r
        this.g = g
        this.b = b
        this.a = a
        this.depth = depth
    }
}

export class Main {
    constructor(document, window) {
        this.document = document
        this.window = window

        this.canvas = this.document.createElement('canvas')
        this.document.body.appendChild(this.canvas)
        this.context = this.canvas.getContext('2d')

        this.scale = 16
        this.font_scale = 4

        this.context.canvas.width = this.window.innerWidth
        this.context.canvas.height = this.window.innerHeight

        this.width = Math.ceil(this.window.innerWidth / this.scale)
        this.height = Math.ceil(this.window.innerHeight / this.scale)

        this.context.imageSmoothingEnabled = false

        this.texture_contexts = {}
        this.tris = []

        this.create_font_context()

        this.antialiasing = false

        this.keys = {}
        this.speed = 0.1
        this.mouse_x = 0
        this.mouse_y = 0

        // this.camera_vector = new Vector(0, 2.1, -2)
        // this.angle_vector = new Vector(0, 0, 0)

        this.camera_vector = new Vector(-4.4, 4.7, -4)
        this.angle_vector = new Vector(31.8 * Math.PI / 180, 48.5 * Math.PI / 180, 0)

        this.default_fov = 70
        this.field_of_view_degrees = this.default_fov
        this.default_zoomed_fov = 30
        this.zoomed_fov = this.default_zoomed_fov
        this.min_fov = 10
        this.max_fov = 120

        this.document.onkeydown = event => {
            this.keys[event.code] = 1
        }

        this.document.onkeyup = event => {
            delete this.keys[event.code]
        }

        this.document.onmousemove = event => {
            if (document.pointerLockElement === this.canvas) {
                this.mouse_x += event.movementX * 2.0
                this.mouse_y += event.movementY * 2.0

                if (this.mouse_y / this.height < -0.5 * Math.PI) {
                    this.mouse_y = -0.5 * Math.PI * this.height
                }

                if (this.mouse_y / this.height > 0.5 * Math.PI) {
                    this.mouse_y = 0.5 * Math.PI * this.height
                }

                this.angle_vector.x = this.mouse_y / this.height
                this.angle_vector.y = this.mouse_x / this.width
            }
        }

        this.document.onwheel = event => {
            if ('KeyC' in this.keys) {
                if (event.deltaY > 0) {
                    this.zoomed_fov = Math.min(this.zoomed_fov + 5, this.max_fov)
                } else {
                    this.zoomed_fov = Math.max(this.zoomed_fov - 5, this.min_fov)
                }
            }
        }

        this.canvas.onclick = () => this.canvas.requestPointerLock()
    }

    update() {
        let m = [[0.0], [0.0]]
        if ('KeyW' in this.keys) {
            m[1][0] += this.speed
        }
        if ('KeyS' in this.keys) {
            m[1][0] -= this.speed
        }
        if ('KeyA' in this.keys) {
            m[0][0] += this.speed
        }
        if ('KeyD' in this.keys) {
            m[0][0] -= this.speed
        }
        if ('Space' in this.keys) {
            this.camera_vector.y += this.speed
        }
        if ('ShiftLeft' in this.keys) {
            this.camera_vector.y -= this.speed
        }
        if ('KeyC' in this.keys) {
            this.field_of_view_degrees = this.zoomed_fov
        } else {
            this.field_of_view_degrees = this.default_fov
            this.zoomed_fov = this.default_zoomed_fov
        }
        let pos = this.rotate(this.angle_vector.y, m)
        this.camera_vector.x += -1 * pos.get_row(0)
        this.camera_vector.z += pos.get_row(1)

        this.draw_frame_buffer()

        requestAnimationFrame(() => this.update())
    }

    rotate(theta, pos) {
        return new Matrix([[Math.cos(theta), -1 * Math.sin(theta)], [Math.sin(theta), Math.cos(theta)]])
            .multiply(new Matrix(pos))
    }

    initialize_tris(tris) {
        this.tris = tris
        let texture_names = []

        for (let tri of tris) {
            if (!(tri.texture in texture_names)) {
                texture_names.push(tri.texture)
            }
        }

        this.create_texture_contexts(texture_names)
    }

    create_texture_contexts(texture_names) {
        for (let texture_name of texture_names) {
            if (!this.texture_contexts[texture_name]) {
                let image = new Image()

                const image_canvas = document.createElement('canvas')
                const image_context = image_canvas.getContext('2d', { willReadFrequently: true })

                image.onload = () => {
                    image_context.canvas.width = image.width
                    image_context.canvas.height = image.height

                    image_context.drawImage(image, 0, 0)

                    this.texture_contexts[texture_name] = image_context
                }

                image.src = `assets/images/blocks/${texture_name}.png`
            }
        }
    }

    create_font_context() {
        let image = new Image()

        image.src = 'assets/images/gui/font/font.png'

        this.font_image = image
    }

    get_font_image(string) {
        let characters = [
            [ [' ', 3], ['!', 1], ['"', 4], ['#', 5], ['$', 5], ['%', 5], ['&', 5], ['\'', 2],['(', 4], [')', 4], ['*', 4], ['+', 5], [',', 1], ['-', 5], ['.', 1], ['/', 5] ],
            [ ['0', 5], ['1', 5], ['2', 5], ['3', 5], ['4', 5], ['5', 5], ['6', 5], ['7', 5], ['8', 5], ['9', 5], [':', 1], [';', 1], ['<', 4], ['=', 5], ['>', 4], ['?', 5] ],
            [ ['@', 6], ['A', 5], ['B', 5], ['C', 5], ['D', 5], ['E', 5], ['F', 5], ['G', 5], ['H', 5], ['I', 3], ['J', 5], ['K', 5], ['L', 5], ['M', 5], ['N', 5], ['O', 5] ],
            [ ['P', 5], ['Q', 5], ['R', 5], ['S', 5], ['T', 5], ['U', 5], ['V', 5], ['W', 5], ['X', 5], ['Y', 5], ['Z', 5], ['[', 3], ['\\', 5],[']', 3], ['^', 5], ['_', 5] ],
            [ ['`', 2], ['a', 5], ['b', 5], ['c', 5], ['d', 5], ['e', 5], ['f', 4], ['g', 5], ['h', 5], ['i', 2], ['j', 5], ['k', 4], ['l', 2], ['m', 5], ['n', 5], ['o', 5] ],
            [ ['p', 5], ['q', 5], ['r', 5], ['s', 5], ['t', 3], ['u', 5], ['v', 5], ['w', 5], ['x', 5], ['y', 5], ['z', 5], ['{', 4], ['|', 2], ['}', 4], ['~', 6] ],
        ]

        let width = 0
        let image_coordinates = []

        for (let character of string) {
            for (let row = 0; row < characters.length; row++) {
                for (let character_map = 0; character_map < characters[row].length; character_map++) {
                    if (character === characters[row][character_map][0]) {
                        width += characters[row][character_map][1] + 1
                        image_coordinates.push([row, character_map, characters[row][character_map][1]])
                    }
                }
            }
        }

        let string_canvas = document.createElement('canvas')
        let string_context = string_canvas.getContext('2d', { willReadFrequently: true })

        string_context.canvas.width = width
        string_context.canvas.height = 8

        let width_pos = 0

        for (let coordinate of image_coordinates) {
            string_context.drawImage(this.font_image, 8 * coordinate[1], 8 * coordinate[0], coordinate[2], 8, width_pos, 0, coordinate[2], 8)

            width_pos += coordinate[2] + 1
        }

        return string_canvas
    }

    draw_text(string, x, y) {
        let image_canvas = this.get_font_image(string)

        this.context.drawImage(image_canvas, x * this.font_scale, y * this.font_scale, image_canvas.width * this.font_scale, image_canvas.height * this.font_scale)
    }

    get_clipped_tris(tris) {
        let side_length = Math.tan(this.field_of_view_degrees * Math.PI / 360)

        let near_n = new Vector(0, 0, 1)
        let far_n = new Vector(0, 0, -1)
        let left_n = new Vector(1, 0, side_length * this.width / this.height)
        let right_n = new Vector(-1, 0, side_length * this.width / this.height)
        let top_n = new Vector(0, 1, side_length)
        let bottom_n = new Vector(0, -1, side_length)

        let clip = Tri.clip_tris(near_n, -0.0001, tris)
        clip = Tri.clip_tris(far_n, 10000, clip)
        clip = Tri.clip_tris(left_n, 0, clip)
        clip = Tri.clip_tris(right_n, 0, clip)
        clip = Tri.clip_tris(top_n, 0, clip)
        clip = Tri.clip_tris(bottom_n, 0, clip)

        return clip
    }

    project_tris() {
        let screen_space_tris = []

        for (let tri of this.tris) {
            let v0 = tri.v0.to_screen_space(this.camera_vector, this.angle_vector)
            let v1 = tri.v1.to_screen_space(this.camera_vector, this.angle_vector)
            let v2 = tri.v2.to_screen_space(this.camera_vector, this.angle_vector)

            screen_space_tris.push(new Tri(v0, v1, v2, tri.texture))
        }

        let clipped_tris = this.get_clipped_tris(screen_space_tris)

        let projections = []

        for (let tri of clipped_tris) {
            let v0 = tri.v0.perspective_project(tri.v0.vector.to_matrix(), this.field_of_view_degrees, this.width, this.height)
            let v1 = tri.v1.perspective_project(tri.v1.vector.to_matrix(), this.field_of_view_degrees, this.width, this.height)
            let v2 = tri.v2.perspective_project(tri.v2.vector.to_matrix(), this.field_of_view_degrees, this.width, this.height)

            projections.push(new ProjectedTri(v0, v1, v2, tri))
        }

        return projections
    }

    get_frame_buffer() {
        let projected_tris = this.project_tris()

        let frame_buffer = new FrameBuffer(this.width, this.height)

        for (let tri of projected_tris) {
            let texture_context = this.texture_contexts[tri.texture]
            let texture_width = texture_context.canvas.width
            let texture_height = texture_context.canvas.height

            for (let y = 0; y < this.height; y++) {
                for (let x = 0; x < this.width; x++) {
                    let weights = tri.get_weights(new XY(x + 0.5, y + 0.5))

                    if (tri.is_inside(weights)) {
                        let scaled_weights = tri.get_scaled_weights(weights)

                        let depth_scale = tri.get_depth_scale(scaled_weights)

                        let depth = tri.get_value(tri.v0.z, tri.v1.z, tri.v2.z, scaled_weights, depth_scale)

                        if (depth < frame_buffer.get(x, y).depth) {
                            let b = tri.get_value(tri.v0.b, tri.v1.b, tri.v2.b, scaled_weights, depth_scale)

                            let u = tri.get_value(tri.v0.u, tri.v1.u, tri.v2.u, scaled_weights, depth_scale)
                            let v = tri.get_value(tri.v0.v, tri.v1.v, tri.v2.v, scaled_weights, depth_scale)

                            let color_data = texture_context.getImageData(
                                Math.min(texture_width - 1, Math.floor(u * texture_width)),
                                Math.min(texture_height - 1, Math.floor(texture_height - v * texture_height)),
                                1, 1
                            )

                            let red = color_data.data[0] * (b / 255)
                            let green = color_data.data[1] * (b / 255)
                            let blue = color_data.data[2] * (b / 255)
                            let alpha = color_data.data[3] / 255

                            red = red * alpha + frame_buffer.get(x, y).r * (1 - alpha)
                            green = green * alpha + frame_buffer.get(x, y).g * (1 - alpha)
                            blue = blue * alpha + frame_buffer.get(x, y).b * (1 - alpha)

                            frame_buffer.set(x, y, new Color(Math.round(red), Math.round(green), Math.round(blue), 255, depth))
                        }
                    }
                }
            }
        }

        return this.antialiasing ? this.antialias_frame_buffer(frame_buffer) : frame_buffer
    }

    antialias_frame_buffer(buffer) {
        let new_buffer = new FrameBuffer(this.width, this.height)

        for (let y = 0; y < this.height - 1; y++) {
            for (let x = 0; x < this.width - 1; x++) {
                let red =
                    (
                        buffer.get(x, y).r +
                        buffer.get(x, y + 1).r +
                        buffer.get(x + 1, y).r +
                        buffer.get(x + 1, y + 1).r
                    ) / 4
                let green =
                    (
                        buffer.get(x, y).g +
                        buffer.get(x, y + 1).g +
                        buffer.get(x + 1, y).g +
                        buffer.get(x + 1, y + 1).g
                    ) / 4
                let blue =
                    (
                        buffer.get(x, y).b +
                        buffer.get(x, y + 1).b +
                        buffer.get(x + 1, y).b +
                        buffer.get(x + 1, y + 1).b
                    ) / 4
                new_buffer.set(x, y, new Color(red, green, blue, buffer.get(x, y).a, buffer.get(x, y).depth))
            }
        }

        return new_buffer
    }

    draw_frame_buffer() {
        let image_data = this.get_frame_buffer().to_image_data()

        let prev_image_data = this.context.createImageData(this.width, this.height)

        for (let i = 0; i < image_data.length; i++) {
            prev_image_data.data[i] = image_data[i]
        }

        let new_canvas = this.document.createElement('canvas')
        let new_context = new_canvas.getContext('2d')

        new_context.canvas.width = this.width
        new_context.canvas.height = this.height

        new_context.putImageData(prev_image_data, 0, 0)

        this.context.drawImage(new_context.canvas, 0, 0, this.width * this.scale, this.height * this.scale)

        this.draw_text(`Pos: X: ${parseFloat(this.camera_vector.x.toFixed(4))} Y: ${parseFloat(this.camera_vector.y.toFixed(4))} Z: ${parseFloat(this.camera_vector.z.toFixed(4))}`, 2, 2)
        this.draw_text(`Cam: X: ${parseFloat((this.angle_vector.x / Math.PI * 180).toFixed(4))} Y: ${parseFloat((this.angle_vector.y / Math.PI * 180).toFixed(4))}`, 2, 12)
    }
}

let main = new Main(document, window)

function transform(tris, x, y, z) {
    let new_tris = JSON.parse(JSON.stringify(tris))

    for (let tri of new_tris) {
        tri.v0 = new Vertex(tri.v0.x + x, tri.v0.y + y, tri.v0.z + z, tri.v0.u, tri.v0.v, tri.v0.b)
        tri.v1 = new Vertex(tri.v1.x + x, tri.v1.y + y, tri.v1.z + z, tri.v1.u, tri.v1.v, tri.v1.b)
        tri.v2 = new Vertex(tri.v2.x + x, tri.v2.y + y, tri.v2.z + z, tri.v2.u, tri.v2.v, tri.v2.b)
    }

    return new_tris
}

function cube(x, y, z, texture_front, texture_back, texture_left, texture_right, texture_top, texture_bottom) {
    let cube = Tri.of([
        [Vertex.of(-0.5,  0.5, -0.5, 0, 1, 191), Vertex.of(-0.5, -0.5, -0.5, 0, 0, 191), Vertex.of( 0.5,  0.5, -0.5, 1, 1, 191), texture_front],
        [Vertex.of( 0.5, -0.5, -0.5, 1, 0, 191), Vertex.of( 0.5,  0.5, -0.5, 1, 1, 191), Vertex.of(-0.5, -0.5, -0.5, 0, 0, 191), texture_front],
        [Vertex.of( 0.5,  0.5,  0.5, 0, 1, 191), Vertex.of( 0.5, -0.5,  0.5, 0, 0, 191), Vertex.of(-0.5,  0.5,  0.5, 1, 1, 191), texture_back],
        [Vertex.of(-0.5, -0.5,  0.5, 1, 0, 191), Vertex.of(-0.5,  0.5,  0.5, 1, 1, 191), Vertex.of( 0.5, -0.5,  0.5, 0, 0, 191), texture_back],
        [Vertex.of(-0.5,  0.5,  0.5, 0, 1, 159), Vertex.of(-0.5, -0.5,  0.5, 0, 0, 159), Vertex.of(-0.5,  0.5, -0.5, 1, 1, 159), texture_left],
        [Vertex.of(-0.5, -0.5, -0.5, 1, 0, 159), Vertex.of(-0.5,  0.5, -0.5, 1, 1, 159), Vertex.of(-0.5, -0.5,  0.5, 0, 0, 159), texture_left],
        [Vertex.of( 0.5,  0.5, -0.5, 0, 1, 159), Vertex.of( 0.5, -0.5, -0.5, 0, 0, 159), Vertex.of( 0.5,  0.5,  0.5, 1, 1, 159), texture_right],
        [Vertex.of( 0.5, -0.5,  0.5, 1, 0, 159), Vertex.of( 0.5,  0.5,  0.5, 1, 1, 159), Vertex.of( 0.5, -0.5, -0.5, 0, 0, 159), texture_right],
        [Vertex.of(-0.5,  0.5,  0.5, 0, 1, 255), Vertex.of(-0.5,  0.5, -0.5, 0, 0, 255), Vertex.of( 0.5,  0.5,  0.5, 1, 1, 255), texture_top],
        [Vertex.of( 0.5,  0.5, -0.5, 1, 0, 255), Vertex.of( 0.5,  0.5,  0.5, 1, 1, 255), Vertex.of(-0.5,  0.5, -0.5, 0, 0, 255), texture_top],
        [Vertex.of( 0.5, -0.5,  0.5, 0, 1, 95),  Vertex.of( 0.5, -0.5, -0.5, 0, 0, 95),  Vertex.of(-0.5, -0.5,  0.5, 1, 1, 95),  texture_bottom],
        [Vertex.of(-0.5, -0.5, -0.5, 1, 0, 95),  Vertex.of(-0.5, -0.5,  0.5, 1, 1, 95),  Vertex.of( 0.5, -0.5, -0.5, 0, 0, 95),  texture_bottom],
    ])

    return transform(cube, x, y, z)
}

function cube_all(x, y, z, texture) {
    return cube(x, y, z, texture, texture, texture, texture, texture, texture)
}

function cube_top_side_bottom(x, y, z, texture_top, texture_side, texture_bottom) {
    return cube(x, y, z, texture_side, texture_side, texture_side, texture_side, texture_top, texture_bottom)
}

function cube_top_side(x, y, z, texture_top, texture_side) {
    return cube(x, y, z, texture_side, texture_side, texture_side, texture_side, texture_top, texture_top)
}

function stone_block(x, y, z) {
    return cube_all(x, y, z, 'stone')
}

function dirt_block(x, y, z) {
    return cube_all(x, y, z, 'dirt')
}

function grass_block(x, y, z) {
    return cube_top_side_bottom(x, y, z, 'grass_top_beta', 'grass_side_beta', 'dirt')
}

function log_block(x, y, z) {
    return cube_top_side(x, y, z, 'log_top', 'log_side')
}

function leaf_block(x, y, z) {
    return cube_all(x, y, z, 'leaf')
}

function glass_block(x, y, z) {
    return cube_all(x, y, z, 'glass')
}

function wood_block(x, y, z) {
    return cube_all(x, y, z, 'wood')
}

let cubes = [
    ...stone_block(-1, -2, -1),
    ...stone_block(-1, -2, 0),
    ...stone_block(-1, -2, 1),
    ...stone_block(0, -2, -1),
    ...stone_block(0, -2, 0),
    ...stone_block(0, -2, 1),
    ...stone_block(1, -2, -1),
    ...stone_block(1, -2, 0),
    ...stone_block(1, -2, 1),

    ...dirt_block(-1, -1, -1),
    ...dirt_block(-1, -1, 0),
    ...dirt_block(-1, -1, 1),
    ...dirt_block(0, -1, -1),
    ...dirt_block(0, -1, 0),
    ...dirt_block(0, -1, 1),
    ...dirt_block(1, -1, -1),
    ...dirt_block(1, -1, 0),
    ...dirt_block(1, -1, 1),

    ...grass_block(-1, 0, -1),
    ...grass_block(-1, 0, 0),
    ...grass_block(-1, 0, 1),
    ...grass_block(0, 0, -1),
    ...grass_block(0, 0, 0),
    ...grass_block(0, 0, 1),
    ...grass_block(1, 0, -1),
    ...grass_block(1, 0, 0),
    ...grass_block(1, 0, 1),

    ...log_block(0, 1, 0),
    ...log_block(0, 2, 0),
    ...log_block(0, 3, 0),

    ...leaf_block(-1, 3, -1),
    ...leaf_block(-1, 3, 0),
    ...leaf_block(-1, 3, 1),
    ...leaf_block(0, 3, -1),
    ...leaf_block(0, 3, 1),
    ...leaf_block(1, 3, -1),
    ...leaf_block(1, 3, 0),
    ...leaf_block(1, 3, 1),
    ...leaf_block(-1, 4, 0),
    ...leaf_block(1, 4, 0),
    ...leaf_block(0, 4, 0),
    ...leaf_block(0, 4, 1),
    ...leaf_block(0, 4, -1),

    // ...wood_block(1, 1, -1),
    //
    // ...glass_block(0, 1, -1),
]

main.initialize_tris(cubes)

setTimeout(() => {
    main.update()
}, 1000)

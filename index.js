// ====Variable====: let, const
let firstNum = 6;
firstNum = 7;

const HOURS_IN_DAY = 24;
// HOURS_IN_DAY = 21; --throw an error--

// Arrow function
const multipleTen = (num) => {
    return num * 10;
}
// const multipleTen = (num) => num * 10;
// const multipleTen = num => num * 10;

// // default parameters
// const multipleTen = (num = 1) => num * 10;

// Template String
const nameAnimal = 'Lion';
const xinChao = `
    Hello World,
    My name is ${nameAnimal}. 
`

// Rest parameters
const caculateAvg = (...nums) => {
    let total = 0;
    let length = nums.length;

    for (let num of nums) {
        total += num;
    }

    return total / length;
}

console.log(caculateAvg(1, 2, 3))
console.log(caculateAvg(1, 3, 5, 7, 9, 11, 13))

// Spread
const users = ['Dai', 'Duc', 'Nguyen'];
const copyUsers = [...users];
/**
 * ...users => 'Dai', 'Duc', 'Nguyen'
 */
const users_2 = [...users, 'cho', 'meo'];

const hondaCar = {
    model: 'Honda',
    weight: '200Kg',
    price: '800 milion'
}

const redCar = {
    ...hondaCar,
    color: 'Red',
}
/*
    ...hondaCar =>  model: 'Honda',
                    weight:'200Kg',
                    price: '800 milion',
    color: 'Red',
*/

// Destructuring

// ====Array====
let a = 10;
let b = 20;
// ====swap====
[a, b] = [b, a]

const findIndexAndMax = arr => {
    let max = arr[0];
    let index = 0;
    for (let i = 1; i < arr.length; i++) {
        if (max < arr[i]) {
            max = arr[i];
            index = i
        }
    }
    return [index, max]
}
const numbers = [1, 2, 4, 7, 6, 9]
let [position, value] = findIndexAndMax(numbers)
console.log(position);// 5
console.log(value); // 9

//====Object====
const house = {
    color: 'Gray',
    width: '5m',
    height: '20m',
}
let { color, width, height } = house;
/**
 * Tương ứng với
 * color => house.color
 * width => house.height
 * height => house.width
 */
console.log(color); // 'Gray'
console.log(width); // '5m'
console.log(height); // '20m'

const printInfoHouse = ({ color, width, height }) => {
    console.log(`
    Nhà màu ${color} có chiều rộng là ${width} 
    và chiều dài là ${height}
    `)
}

// const printInfoHouse = ({ color = 'Đen', width = '1m', height = '5m' }) => {
//     console.log(`
//     Nhà màu ${color} có chiều rộng là ${width} 
//     và chiều dài là ${height}
//     `)
// }
printInfoHouse(house)
const fs = require('fs');

const file = fs.readFileSync(process.argv[2])
    .toString()
    .split('\n');

console.log('Testing');
const testingLines = file.filter(l => l.startsWith('Testing'));
for (const l of testingLines) {
    console.log(l.match(/Testing (.*)/)[1])
}

console.log('Depth');
const depthLines = file.filter(l => l.match(/^\d+::Depth/) !== null);
for (const l of depthLines) {
    console.log(l.match(/(\d+)/)[1])
}

console.log('Gates');
const gatesLines = file.filter(l => l.match(/^\d+::Gates/) !== null);
for (const l of gatesLines) {
    console.log(l.match(/(\d+)/)[1])
}

console.log('Real');
const realLines = file.filter(l => l.startsWith('real'));
for (const l of realLines) {
    // console.log(l);
    const time = l.match(/real[\s]+(\d+)m([\d\.]+)s/);
    // console.log(`${time[1]}m${time[2]}s`)
    console.log((Number(time[1] * 60) + Number(time[2])) * 1000)
}

console.log('User + Sys');
const userLines = file.filter(l => l.startsWith('user'));
const sysLines = file.filter(l => l.startsWith('sys'));
for (let i = 0; i < userLines.length; i++) {
    const userTime = userLines[i].match(/^user[\s]+(\d+)m([\d\.]+)s/);
    const sysTime = sysLines[i].match(/^sys[\s]+(\d+)m([\d\.]+)s/);
    console.log(
        (Number(userTime[1] * 60) + Number(userTime[2])) * 1000 +
        (Number(sysTime[1] * 60) + Number(sysTime[2])) * 1000)
}


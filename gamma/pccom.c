
byte PC_spcl(byte info) {
  switch (info) {
    case 0xE1:
    case 0xE2:
    case 0xE3:
    case 0xE4:
    case 0xE5:
    case 0xE6:
    case 0xE7:
    case 0xE9:
    case 0xEA:
    case 0xEE:
      return 1;
      break;
    default:
      return 0;
  }
}

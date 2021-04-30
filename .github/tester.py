from libtester import prepare, SR
import contextlib
import io
import os
import random
import traceback
import sys

def main():
    e = prepare()
    if not e:
        print("Программа не скомпилирована")
        sys.exit(1)

    os.environ["ASAN_OPTIONS"] = "exitcode=154"

    errors = 0

    def testcase(arg1, arg2, expect_error=False):
        log = io.StringIO()
        with contextlib.redirect_stdout(log):
            result = e.expect_success(f"{str(arg1)} {str(arg2)}", arguments=[arg1, arg2])
        if not result:
            return result
        print(f"Ввод: {str(arg1)} {str(arg2)}", end='\t')
        value = arg1 * arg2
        try:
            output = float(result.stdout)
            print("Вывод: ", output, end='\t')
            if abs(value) < 1e-16 and abs(output) > 1e-16:
                print("ОШИБКА: ожидается {0:.16e}".format(value))
                return expect_error
            elif abs(value) > 1e-16 and abs((output - value) / value) > 1e-14:
                print("ОШИБКА: ожидается {0:.16e}".format(value), end="\t")
                print("Погрешность: ", abs((output - value) / value))
                return expect_error
        except Exception:
            if not expect_error:
                print(f"ОШИБКА: Не удалось разобрать вывод программы: |{result.stdout[:30]}|")
                traceback.print_exc()
            return expect_error
        
        if not expect_error:
            print("OK")
            return result
        else:
            return False

    for _ in range(200):
        errors += not testcase(random.expovariate(0.001), random.expovariate(0.001))
        errors += not testcase(random.expovariate(-0.001), random.expovariate(0.001))
        errors += not testcase(random.paretovariate(0.1), random.paretovariate(0.1))
    
    for _ in range(10):
        errors += not testcase(random.paretovariate(0.01), random.paretovariate(0.01))

    print(f"Тестирование завершено, количество ошибок: {errors}")

    sys.exit(1 if errors > 0 else 0)

main()

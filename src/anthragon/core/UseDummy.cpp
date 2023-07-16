/*
 * Anthragon C++ GUI library
 * (c) Copyright 2023 Moxibyte GmbH
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the MIT License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * MIT License for more details.
 *
 * You should have received a copy of the MIT License along with this
 * program. If not, see <https://opensource.org/license/mit/>.
 *
 */
#include <anthragon/core/UseDummy.h>

void UseDummy()
{
    IDummy* dummy = nullptr;
    if (dummy)
    {
        dummy->CallDummy();
    }
}
